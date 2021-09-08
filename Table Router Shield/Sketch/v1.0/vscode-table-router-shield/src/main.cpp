/**
 * Table Rputer Shield Sketch v1.0
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 * 
 * ***********************    General Notes    *****************************
 * - Uncomment means removing the 2 // in front of #define.
 * 
 * ***********************    Changelog    *********************************
 * # 1.0
 * - initial release
 * 
**/
#include <Arduino.h>
// The Rotary Encoder library should be included with this firmware
#include "RotaryEncoder.h"
// AccelStepper library for controlling the stepper motor
#include <AccelStepper.h>
// Display library
// #include <U8x8lib.h>
// Debouncing library
#include <Bounce2.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define DISPLAY_REFRESH_INTERVAL_MS 200
#define ENCODER_READ_INTERVAL_MS 100

// Pin configuration
const byte buttonProbePin = 36;
const byte limitBottomPin = 39;
const byte limitTopPin = 34;
const byte limitProbePin = 35;
const byte potiRotationSpeedPin = 32;
const byte spindlePwmPin = 33;
const byte pullPin = 25;
const byte dirPin = 26;
const byte enaPin = 27;
const byte d14Pin = 14;
const byte buttonSpindleStartPin = 12;
const byte d13Pin = 13;

const byte buttonDirectPin = 23;
const byte encoderBPin = 19;
const byte encoderAPin = 18;
const byte buttonOkPin = 5;
const byte d17Pin = 17;
const byte d16Pin = 16;
const byte spindleOnOffPin = 4;
const byte d2Pin = 2;
const byte buttonWZWPin = 15;

// Spindle & Motor configuration
const byte spindlePitch = 4;             // How does it move with one turn around - in mm
const uint16_t motorStepsPerCycle = 1600; // 1600 means 200 steps(normal motor steps per cyle) x 8(microstepping 1/8 step) = 1600

const uint16_t fastSpeed = 5; // in mm per second
const uint16_t slowerSpeed = 1; // in mm per second


uint16_t acceleration = 0;
uint16_t maxSpeed = 0;
uint16_t slowSpeed = 0;

const byte displaySecondColumnOffset = 8;

AccelStepper stepper(AccelStepper::DRIVER, pullPin, dirPin);
RotaryEncoder encoder(encoderAPin, encoderBPin);
// U8X8_SSD1306_128X64_NONAME_HW_I2C display(/* reset=*/U8X8_PIN_NONE);
Bounce buttonProbe = Bounce();
Bounce buttonOk = Bounce();
Bounce buttonWZW = Bounce();
Bounce buttonDirect = Bounce();
Bounce limitTop = Bounce();
Bounce limitBottom = Bounce();
Bounce buttonSpindleStart = Bounce();
TaskHandle_t DisplayTask;

enum STATES
{
    NONE,
    WZW,
    MOVING,
    PROBING,
    PROBE_ERROR,
    DIRECT,
    LIMIT_TOP_ERROR,
    LIMIT_BOTTOM_ERROR,
    LIMIT_TOP_TRIGGERED,
    LIMIT_BOTTOM_TRIGGERED
};

typedef struct
{
    STATES lastState;
    float lastCurrentPosition;
    float lastTargetPosition;
} DISPLAY_STATE;

DISPLAY_STATE displayState = {
    STATES::NONE,
    0.0,
    0.0};

// Variables
int64_t lastEncoderPosition = 0;
int newPos_mm = -20;
uint32_t _lastDisplayUpdate = DISPLAY_REFRESH_INTERVAL_MS;
uint32_t _lastEncoderRead = ENCODER_READ_INTERVAL_MS;
float targetPos_mm = 0;
STATES actualState = NONE;

float readEncoder();
void moveToPosMM(float pos_mm, bool setState = true);
void updateDisplay(bool force);
float getPosInMM();
float getStepsPerMM();
void checkButtons();
void pwmTest();
void displayInit();
void quickStop();
void checkLimits();
void driveDowmFromLimitSwitch();
void wzw();
void resetState();
void moveMM(float distance_mm, bool addToActualTarget = false);
void DisplayTaskCode(void *pvParameters);
void probe();
void moveAgainstProbeLimit(uint16_t speed);
float getTargetPosInMM();

void setup()
{
    Serial.begin(115200);
    pinMode(enaPin, OUTPUT);
    pinMode(spindleOnOffPin, OUTPUT);

    digitalWrite(enaPin, LOW);
    digitalWrite(spindleOnOffPin, LOW);

    // Setup PWM
    ledcSetup(0, 5000, 8);
    ledcAttachPin(spindlePwmPin, 0);

    xTaskCreatePinnedToCore(
        DisplayTaskCode, /* Task function. */
        "DisplayTask",   /* name of task. */
        10000,           /* Stack size of task */
        NULL,            /* parameter of the task */
        1,               /* priority of the task */
        &DisplayTask,    /* Task handle to keep track of created task */
        0);

    // Buttons Setup
    buttonOk.attach(buttonOkPin, INPUT_PULLUP);
    buttonOk.interval(10); // interval in ms
    buttonWZW.attach(buttonWZWPin, INPUT_PULLUP);
    buttonWZW.interval(10); // interval in ms
    buttonProbe.attach(buttonProbePin, INPUT_PULLUP);
    buttonProbe.interval(10); // interval in ms
    buttonDirect.attach(buttonDirectPin, INPUT_PULLUP);
    buttonDirect.interval(10); // interval in ms
    buttonSpindleStart.attach(buttonSpindleStartPin, INPUT_PULLUP);
    buttonSpindleStart.interval(10); // interval in ms

    // Limit switches setup
    limitBottom.attach(limitBottomPin, INPUT_PULLUP);
    limitBottom.interval(1); // interval in ms
    limitTop.attach(limitTopPin, INPUT_PULLUP);
    limitTop.interval(1); // interval in ms

    pinMode(limitProbePin, INPUT_PULLUP);

    // Set speed limits according to config
    acceleration = fastSpeed * getStepsPerMM();
    maxSpeed = fastSpeed * getStepsPerMM();
    slowSpeed = slowerSpeed * getStepsPerMM();
    stepper.setPinsInverted(true);
    stepper.setMaxSpeed(maxSpeed);
    stepper.setAcceleration(acceleration);

    resetState();

    // pwmTest();
}

void loop()
{
    // Check limit switches at very first
    checkLimits();
    if (actualState == STATES::LIMIT_TOP_ERROR || actualState == STATES::LIMIT_BOTTOM_ERROR)
    {
        return;
    }

    encoder.tick();
    checkButtons();
    float newDistance = readEncoder();

    if (actualState == STATES::DIRECT && newDistance != 0)
    {
        moveMM(newDistance, true);
    }

    stepper.run();

    if (actualState == STATES::MOVING && stepper.distanceToGo() == 0)
    {
        resetState();
    }
}

void DisplayTaskCode(void *pvParameters)
{
    displayInit();

    for (;;)
    {
        updateDisplay(false);
        vTaskDelay(1);
    }
}

// Returns the change in mm
float readEncoder()
{
    if (millis() - _lastEncoderRead > ENCODER_READ_INTERVAL_MS)
    {
        // Calculate encoder steps
        int16_t actualEncoderPosition = encoder.getPosition();
        if (actualEncoderPosition != lastEncoderPosition)
        {
            int encoderMove = actualEncoderPosition - lastEncoderPosition;
            lastEncoderPosition = actualEncoderPosition;

            float encoderAccelerated = 0;
            uint8_t encoderDelta = encoderMove;
            // Encoder acceleration
            if (encoderDelta > 10)
            {
                encoderAccelerated = encoderMove * 2;
            }
            else if (encoderDelta > 20)
            {
                encoderAccelerated = encoderMove * 3;
            }
            else if (encoderDelta > 30)
            {
                encoderAccelerated = encoderMove * 5;
            }
            else
            {
                encoderAccelerated = encoderMove;
            }

            targetPos_mm += encoderAccelerated * 0.05;

            if (actualState == STATES::DIRECT)
            {
                return encoderAccelerated * 0.05;
            }
        }

        _lastEncoderRead = millis();
    }
    return 0;
}

void moveMM(float distance_mm, bool addToActualTarget)
{
    float stepsPerMM = getStepsPerMM();
    float newPos = stepsPerMM * distance_mm;

    if (addToActualTarget)
    {
        Serial.print("Adding to current target");
        float delta = stepper.targetPosition() - stepper.currentPosition();
        stepper.move(delta + newPos);
    }
    else
    {
        Serial.print("Moving(setting new target)");
        stepper.move(newPos);
    }

    Serial.print(distance_mm);
    Serial.print(" mm - Pos delta: ");
    Serial.println(newPos);
}

void moveToPosMM(float pos_mm, bool setState)
{
    if (setState)
        actualState = STATES::MOVING;

    float stepsPerMM = getStepsPerMM();
    float newPos = stepsPerMM * pos_mm;
    stepper.moveTo(newPos);

    Serial.print("Steps per mm: ");
    Serial.println(stepsPerMM);
    Serial.print("Moving ");
    Serial.print(pos_mm);
    Serial.print(" mm - Pos: ");
    Serial.println(newPos);
}

void displayInit()
{
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Status:");
    lcd.setCursor(displaySecondColumnOffset, 0);
    lcd.print("warte");

    lcd.setCursor(3, 1);
    lcd.print("Ist:");
    lcd.setCursor(displaySecondColumnOffset, 1);
    lcd.print("0.0");
    lcd.setCursor(18, 1);
    lcd.print("mm");

    lcd.setCursor(2, 2);
    lcd.print("Soll:");
    lcd.setCursor(displaySecondColumnOffset, 2);
    lcd.print("0.0");
    lcd.setCursor(18, 2);
    lcd.print("mm");
}

void updateDisplay(bool force)
{
    if (millis() - _lastDisplayUpdate > DISPLAY_REFRESH_INTERVAL_MS || force)
    {
        // Update state if necessary
        if (displayState.lastState != actualState)
        {
            displayState.lastState = actualState;
            lcd.setCursor(displaySecondColumnOffset, 0);
            uint16_t sizeWritten = 0;
            switch (actualState)
            {
            case STATES::MOVING:
                sizeWritten = lcd.print("fahre");
                break;
            case STATES::PROBING:
                sizeWritten = lcd.print("messung");
                break;
            case STATES::PROBE_ERROR:
                sizeWritten = lcd.print("Err mess.");
                break;
            case STATES::WZW:
                sizeWritten = lcd.print("WZW");
                break;
            case STATES::NONE:
                sizeWritten = lcd.print("warte");
                break;
            case STATES::DIRECT:
                sizeWritten = lcd.print("direct move");
                break;
            case STATES::LIMIT_BOTTOM_ERROR:
                sizeWritten = lcd.print("Err unten");
                break;
            case STATES::LIMIT_TOP_ERROR:
                sizeWritten = lcd.print("Err oben");
                break;
            default:
                sizeWritten = lcd.print("Dunno");
                break;
            }
            lcd.setCursor(displaySecondColumnOffset + sizeWritten, 0);
            if (sizeWritten == 3)
                lcd.print("          ");
            if (sizeWritten == 4)
                lcd.print("         ");
            if (sizeWritten == 5)
                lcd.print("        ");
            if (sizeWritten == 6)
                lcd.print("       ");
            if (sizeWritten == 7)
                lcd.print("      ");
            if (sizeWritten == 8)
                lcd.print("     ");
            if (sizeWritten == 9)
                lcd.print("    ");
            if (sizeWritten == 10)
                lcd.print("  ");
            if (sizeWritten == 11)
                lcd.print(" ");
        }

        // Update target if necessary
        if (displayState.lastTargetPosition != targetPos_mm)
        {
            displayState.lastTargetPosition = targetPos_mm;
            lcd.setCursor(displaySecondColumnOffset, 2);
            uint16_t size = lcd.print(targetPos_mm, 1);
            lcd.setCursor(displaySecondColumnOffset + size, 2);
            lcd.print("  ");
        }

        // Update currentPos if necessary
        float currentPosition_mm = getPosInMM();
        if (displayState.lastCurrentPosition != currentPosition_mm)
        {
            displayState.lastCurrentPosition = currentPosition_mm;
            lcd.setCursor(displaySecondColumnOffset, 1);
            uint16_t size = lcd.print(currentPosition_mm, 1);
            lcd.setCursor(displaySecondColumnOffset + size, 1);
            lcd.print("  ");
        }

        _lastDisplayUpdate = millis();
    }
}

float getPosInMM()
{
    long currentPos = stepper.currentPosition();
    if (currentPos == 0)
        return 0;

    return currentPos / getStepsPerMM();
}

float getTargetPosInMM()
{
    long targetPos = stepper.targetPosition();
    if (targetPos == 0)
        return 0;

    return targetPos / getStepsPerMM();
}

float getStepsPerMM()
{
    return (float)motorStepsPerCycle / spindlePitch;
}

void checkButtons()
{
    buttonOk.update();
    buttonWZW.update();
    buttonProbe.update();
    buttonDirect.update();

    // ********************
    // OK BUTTON
    // ********************
    // Short press handling
    if (buttonOk.rose())
    {
        if (buttonOk.previousDuration() < 1000)
        {
            // Stop the moving progess if button is pressed again
            if (actualState == STATES::MOVING)
            {
                quickStop();
                resetState();
            }
            else
            {
                moveToPosMM(targetPos_mm);
            }
        }
    }
    // long press handling - reset postion
    if (buttonOk.read() == LOW)
    {
        if (buttonOk.currentDuration() > 1000)
        {
            targetPos_mm = 0;
            stepper.setCurrentPosition(0);
            return;
        }
    }

    // ********************
    // WZW BUTTON
    // ********************
    // Short press handling
    if (buttonWZW.rose())
    {
        if (buttonWZW.previousDuration() < 1000)
        {
            Serial.println("Button WZW triggered");
            // Stop the moving progess if button is pressed again
            if (actualState == STATES::WZW)
            {
                quickStop();
                resetState();
            }
            else
            {
                wzw();
            }
        }
    }

    // ********************
    // Probe BUTTON
    // ********************
    // Short press handling
    if (buttonProbe.rose())
    {
        if (buttonProbe.previousDuration() < 1000)
        {
            Serial.println("Button Probe triggered");
            // Stop the moving progess if button is pressed again
            if (actualState == STATES::PROBING || actualState == STATES::PROBE_ERROR)
            {
                quickStop();
                resetState();
            }
            else
            {
                probe();
            }
        }
    }

    // ********************
    // Direct BUTTON/SWITCH
    // ********************
    // Short press handling
    if (buttonDirect.rose() || buttonDirect.fell())
    {
        Serial.println("Button Direct triggered");
        if (actualState == STATES::NONE || actualState == STATES::DIRECT)
        {
            resetState();
        }
    }
}

void pwmTest()
{
    while(true)
    {
        Serial.println("FULL");
        ledcWrite(0, 255);
        delay(10000);
        Serial.println("HALF");
        ledcWrite(0, 125);
        delay(10000);

        digitalWrite(spindleOnOffPin, HIGH);
        delay(1000);
        digitalWrite(spindleOnOffPin, LOW);
        delay(1000);
    }

}

void quickStop()
{
    Serial.println("Quickstop");
    stepper.setAcceleration(10000);
    stepper.stop();
    stepper.runToPosition();
    stepper.setAcceleration(acceleration);
}

void driveDowmFromLimitSwitch()
{
    Serial.println("Drive from limit switch");
    // Move as long in the other direction, until the endstop is not triggered anymore
    while (actualState == STATES::LIMIT_TOP_TRIGGERED || actualState == STATES::LIMIT_BOTTOM_TRIGGERED)
    {
        limitTop.update();
        limitBottom.update();
        stepper.setSpeed(slowSpeed);
        if (actualState == STATES::LIMIT_TOP_TRIGGERED)
        {
            if (limitTop.fell())
            {
                stepper.runToNewPosition(stepper.currentPosition() - 100);
                resetState();
                break;
            }
            stepper.move(-10);
        }
        if (actualState == STATES::LIMIT_BOTTOM_TRIGGERED)
        {
            if (limitBottom.fell())
            {
                stepper.runToNewPosition(stepper.currentPosition() + 100);
                resetState();
                break;
            }
            stepper.move(10);
        }
        stepper.runSpeedToPosition();
    }
}

void checkLimits()
{
    limitTop.update();
    limitBottom.update();
    // Check limit switch wiring when not moving
    if (actualState == STATES::NONE || actualState == STATES::LIMIT_TOP_ERROR || actualState == STATES::LIMIT_BOTTOM_ERROR || actualState == STATES::DIRECT)
    {
        if (limitTop.read() == HIGH)
        {
            actualState = STATES::LIMIT_TOP_ERROR;
        }
        else if (actualState == STATES::LIMIT_TOP_ERROR)
        {
            resetState();
        }

        if (limitBottom.read() == HIGH)
        {
            actualState = STATES::LIMIT_BOTTOM_ERROR;
        }
        else if (actualState == STATES::LIMIT_BOTTOM_ERROR)
        {
            resetState();
        }
        return;
    }

    // Top limit switch triggered
    if (limitTop.rose())
    {
        Serial.println("Limit top triggered");
        actualState = STATES::LIMIT_TOP_TRIGGERED;
        quickStop();
        driveDowmFromLimitSwitch();
    }

    // Bottom limit switch triggered
    if (limitBottom.rose())
    {
        Serial.println("Limit bottom triggered");
        actualState = STATES::LIMIT_BOTTOM_TRIGGERED;
        quickStop();
        driveDowmFromLimitSwitch();
    }
}

void wzw()
{
    Serial.println("Start WZW");
    // Move far until endstop is reached
    stepper.moveTo(1000000);
    actualState = STATES::WZW;
}

void resetState()
{
    if (buttonDirect.read() == LOW)
    {
        actualState = STATES::DIRECT;
    }
    else
    {
        actualState = STATES::NONE;
    }
}

void probe()
{
    Serial.println("Start Probe");
    actualState = STATES::PROBING;

    // move as long as bottom endstop is reached
    while (!limitBottom.rose())
    {
        limitTop.update();
        limitBottom.update();

        stepper.move(-1);
        stepper.setSpeed(maxSpeed);
        stepper.runSpeedToPosition();
    }

    // Move against probe limit switch with max speed
    moveAgainstProbeLimit(maxSpeed);

    // Stop if something went wrong
    if (actualState != STATES::PROBING)
    {
        return;
    }

    // Move a littlebit down
    moveMM(-5);
    stepper.setAcceleration(100000);
    stepper.setSpeed(maxSpeed);
    stepper.runToPosition();
    stepper.setAcceleration(acceleration);

    // Move against probe limit switch with slow speed for better accuracy
    moveAgainstProbeLimit(slowSpeed);

    // Stop if something went wrong
    if (actualState != STATES::PROBING)
    {
        return;
    }

    // Finally reset the position
    stepper.setCurrentPosition(0);
    resetState();
}

void moveAgainstProbeLimit(uint16_t speed)
{
    // limitProbe.update();
    // move as long as limitProbe is reached
    // while (!limitProbe.rose())
    while (digitalRead(limitProbePin) == HIGH)
    {
        limitTop.update();
        limitBottom.update();
        // limitProbe.update();

        // If endstops were reached during probing - abort.
        if (limitBottom.rose() || limitTop.rose())
        {
            actualState = STATES::PROBE_ERROR;
            break;
        }

        stepper.move(1);
        stepper.setSpeed(speed);
        stepper.runSpeedToPosition();
    }
}
