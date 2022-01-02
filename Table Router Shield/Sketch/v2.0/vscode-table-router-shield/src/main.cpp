/**
 * Table Router Shield Sketch v2.0
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 * 
 * ***********************    General Notes    *****************************
 * - Uncomment means removing the 2 // in front of #define.
 * ***********************    Changelog    *********************************
 * see Changelog.md file inside the sketch folder
**/
#include <Arduino.h>
// The Rotary Encoder library should be included with this firmware
#include "RotaryEncoder.h"
// AccelStepper library for controlling the stepper motor
#include <AccelStepper.h>
// Debouncing library
#include <Bounce2.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// The time in milliseconds, the display should refresh. The default value should be fine.
// default: 200
#define DISPLAY_REFRESH_INTERVAL_MS 200
// The time in milliseconds, the encoder steps should be read. The default value should be fine.
// default: 50
#define ENCODER_READ_INTERVAL_MS 50
// The lazy way, if you dont want to switch vcc and gnd of the spindle speed poti
// Possible values: true / false
// default: false
#define INVERT_SPINDLE_SPEED_POTI true
// -------------------------------------
// Spindle and Stepper configuration
// -------------------------------------
// How far does the spindle move with one turn around - in mm
#define SPINDLE_PITCH 4
// Motorsteps per cycle - How many steps have to be done to turn the motor once
// 1600 means, 200 steps(normal motor steps per cyle) x 8(microstepping 1/8 step) = 1600
#define MOTOR_STEPS_PER_CYCLE 800
// Fast speed in mm/second - for normal movement
#define MOVEMENT_FAST_SPEED 10

// ---------------------------------------------------------------------
// END configuration - only edit below if you know what you are doing
// ---------------------------------------------------------------------

// Slow speed in mm/second - for probing movement
#define MOVEMENT_SLOW_SPEED MOVEMENT_FAST_SPEED / 3

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
const byte d14Pin = 14; // unused
const byte buttonSpindleStartPin = 12;
const byte d13Pin = 13; // unused

const byte buttonDirectPin = 23;
const byte encoderBPin = 19;
const byte encoderAPin = 18;
const byte buttonOkPin = 5;
const byte d17Pin = 17; // unused
const byte d16Pin = 16; // unused
const byte spindleOnOffPin = 4;
const byte d2Pin = 2; // unused
const byte buttonWZWPin = 15;

// Step multiplier for the encoder: steps to move = multiplier * encoder steps
const uint16_t multiplier = MOTOR_STEPS_PER_CYCLE / SPINDLE_PITCH / 15;

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
Bounce limitProbe = Bounce();
Bounce buttonSpindleStart = Bounce();
TaskHandle_t DisplayTask;

enum LIMITSTATES 
{
    LIMIT_OK,
    LIMIT_IGNORE,
    LIMIT_TRIGGERED,
    LIMIT_ERROR
};

enum MOVINGSTATES 
{
    DIRECT,
    NORMAL
};

enum STATES
{
    NONE,
    WZW,
    MOVING,
    PROBING,
    PROBE_ERROR,
    SPINDLE_ON
};

enum DISPLAYPAGES 
{
    PAGE_WARNING,
    PAGE_OPERATION
};

typedef struct
{
    MOVINGSTATES lastMovingMode;
    STATES lastState;
    float lastCurrentPosition;
    float lastTargetPosition;
    DISPLAYPAGES lastDisplayPage;
    LIMITSTATES lastTopLimitState;
    LIMITSTATES lastBottomLimitState;
    int lastSpindleSpeed;
} DISPLAY_STATE;

DISPLAY_STATE displayState = {
    MOVINGSTATES::NORMAL,
    STATES::NONE,
    0.0,
    0.0,
    DISPLAYPAGES::PAGE_OPERATION,
    LIMITSTATES::LIMIT_OK,
    LIMITSTATES::LIMIT_OK,
    0
};

typedef struct
{
    int lastLimitTopPinState;
    int lastLimitBottomPinState;
    LIMITSTATES topLimitState;
    LIMITSTATES bottomLimitState;
} LIMIT_SWITCHES_STATE;

LIMIT_SWITCHES_STATE limitSwitchesState = {5,5, LIMITSTATES::LIMIT_OK, LIMITSTATES::LIMIT_OK};

// Variables
int64_t _lastEncoderPosition = 0;
uint32_t _lastDisplayUpdate = DISPLAY_REFRESH_INTERVAL_MS;
uint32_t _lastEncoderRead = ENCODER_READ_INTERVAL_MS;
long targetPosition_steps = 0;
STATES currentMachineState = NONE;
MOVINGSTATES currentMovingMode = MOVINGSTATES::NORMAL;
int currentSpindleSpeed = 0;

void updateButtons();
float convertStepsToMM(long steps);
long convertMMToSteps(float mm);
void updateState();
void showWarningDisplay();
void showOperationDisplay();
void initOperationDisplay();
int readEncoder();
void updateDisplay();
float getStepsPerMM();
void performButtonActions();
void setSpindleSpeed();
void displayInit();
void quickStop();
void checkLimits();
void wzw();
void DisplayTaskCode(void *pvParameters);
void probe();
void moveAgainstProbeLimit(uint16_t speed);

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
    buttonOk.interval(5); // interval in ms
    buttonWZW.attach(buttonWZWPin, INPUT_PULLUP);
    buttonWZW.interval(5); // interval in ms
    buttonProbe.attach(buttonProbePin, INPUT_PULLUP);
    buttonProbe.interval(5); // interval in ms
    buttonDirect.attach(buttonDirectPin, INPUT_PULLUP);
    buttonDirect.interval(5); // interval in ms
    buttonSpindleStart.attach(buttonSpindleStartPin, INPUT_PULLUP);
    buttonSpindleStart.interval(5); // interval in ms

    // Limit switches setup
    limitTop.attach(limitTopPin, INPUT_PULLUP);
    limitTop.interval(10);    
    limitBottom.attach(limitBottomPin, INPUT_PULLUP);
    limitBottom.interval(10);    
    limitProbe.attach(limitProbePin, INPUT_PULLUP);
    limitProbe.interval(1);

    // pinMode(limitProbePin, INPUT_PULLUP);

    // Set speed limits according to config
    acceleration = MOVEMENT_FAST_SPEED * getStepsPerMM() * 2;
    maxSpeed = MOVEMENT_FAST_SPEED * getStepsPerMM();
    slowSpeed = MOVEMENT_SLOW_SPEED * getStepsPerMM();
    stepper.setPinsInverted(true);
    stepper.setMaxSpeed(maxSpeed);
    stepper.setAcceleration(acceleration);

    Serial.print("Setup done!");

    // Set the correct moving mode
    if (buttonDirect.read() == LOW)
    {
        currentMovingMode = MOVINGSTATES::DIRECT;
    }
    else
    {
        currentMovingMode = MOVINGSTATES::NORMAL;
    }
}

void loop()
{
    int probe = digitalRead(limitProbePin);
    if(!probe) {
        Serial.println("ausgelöst");
    }
    setSpindleSpeed();
    // Dont do anything if the spindle is on!
    if(currentMachineState == STATES::SPINDLE_ON) {
        buttonSpindleStart.update();
        // Always give the possibility to turn off the spindle
        if(buttonSpindleStart.read() == HIGH) {
            Serial.println("Spindle turned off");
            digitalWrite(spindleOnOffPin, LOW);
            currentMachineState = STATES::NONE;
        }
        return;
    }
    // Check limit switches at very first
    checkLimits();
    // Update the buttons state
    updateButtons();
    // Perfom action according to button states
    performButtonActions();
    // Check if normal operation should be aborted, for example because of the limit switches
    // or if spindle was switched on - no moving while spindle is switched on
    if(limitSwitchesState.topLimitState == LIMITSTATES::LIMIT_ERROR 
        || limitSwitchesState.topLimitState == LIMITSTATES::LIMIT_TRIGGERED
        || limitSwitchesState.bottomLimitState == LIMITSTATES::LIMIT_ERROR
        || limitSwitchesState.bottomLimitState == LIMITSTATES::LIMIT_TRIGGERED
        || currentMachineState == STATES::SPINDLE_ON) {
        return;
    }
    // update the internal state
    updateState();
    // Check encoder movement
    encoder.tick();
    int encoderMove = readEncoder();
    targetPosition_steps += encoderMove * multiplier;

    if(currentMovingMode == MOVINGSTATES::DIRECT) {
        stepper.moveTo(targetPosition_steps);
    }    

    // Make steps
    stepper.run();
}

// Updates the button state. Has to be called as often as possible (normally every loop)
void updateButtons()
{
    buttonOk.update();
    buttonWZW.update();
    buttonProbe.update();
    buttonDirect.update();
    buttonSpindleStart.update();
}

float convertStepsToMM(long steps)
{
    if (steps == 0)
        return 0;

    return steps / getStepsPerMM();
}

long convertMMToSteps(float mm)
{
    if (mm == 0)
        return 0;

    return mm * getStepsPerMM();
}

void updateState()
{
    if (stepper.isRunning())
    {
        currentMachineState = STATES::MOVING;
    }
    else
    {
        currentMachineState = STATES::NONE;
    }
}

void checkLimits()
{
    limitTop.update();
    limitBottom.update();
    limitProbe.update();

    // Check limit switch wiring when not moving - state is not triggred - state is not ignore
    if (currentMachineState != STATES::MOVING 
        && limitSwitchesState.topLimitState != LIMITSTATES::LIMIT_TRIGGERED 
        && limitSwitchesState.bottomLimitState != LIMITSTATES::LIMIT_TRIGGERED
        && limitSwitchesState.topLimitState != LIMITSTATES::LIMIT_IGNORE
        && limitSwitchesState.bottomLimitState != LIMITSTATES::LIMIT_IGNORE)
    {
        if (limitTop.read() == HIGH)
        {
            limitSwitchesState.topLimitState = LIMITSTATES::LIMIT_ERROR;
        } else {
            limitSwitchesState.topLimitState = LIMITSTATES::LIMIT_OK;
        }

        if (limitBottom.read() == HIGH)
        {
            limitSwitchesState.bottomLimitState = LIMITSTATES::LIMIT_ERROR;
        } else {
            limitSwitchesState.bottomLimitState = LIMITSTATES::LIMIT_OK;
        }
      
        return;
    }

    // Top limit switch changed
    if (limitSwitchesState.lastLimitTopPinState != limitTop.read())
    {
        limitSwitchesState.lastLimitTopPinState = limitTop.read();
        if(limitTop.read() == HIGH) {
            limitSwitchesState.topLimitState = LIMITSTATES::LIMIT_TRIGGERED;
            quickStop();
        } else {
            limitSwitchesState.topLimitState = LIMITSTATES::LIMIT_OK;
        }
    }

    // Bottom limit switch changed
    if (limitSwitchesState.lastLimitBottomPinState != limitBottom.read())
    {
        limitSwitchesState.lastLimitBottomPinState = limitBottom.read();
        if(limitBottom.read() == HIGH) {
            limitSwitchesState.bottomLimitState = LIMITSTATES::LIMIT_TRIGGERED;
            quickStop();
        } else {
            limitSwitchesState.bottomLimitState = LIMITSTATES::LIMIT_OK;
        }
    }
}

void DisplayTaskCode(void *pvParameters)
{
    delay(100);
    displayInit();

    for (;;)
    {
        updateDisplay();
        vTaskDelay(1);
    }
}

// Returns the change in steps
int readEncoder()
{
    if (millis() - _lastEncoderRead > ENCODER_READ_INTERVAL_MS)
    {
        // Calculate encoder steps
        int16_t currentEncoderPosition = encoder.getPosition();
        if (currentEncoderPosition != _lastEncoderPosition)
        {
            int encoderMove = currentEncoderPosition - _lastEncoderPosition;
            _lastEncoderPosition = currentEncoderPosition;

            int encoderAccelerated = 0;
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
            _lastEncoderRead = millis();
            return encoderAccelerated;
        }
        _lastEncoderRead = millis();
    }
    return 0;
}

void displayInit()
{
    lcd.init();
    lcd.backlight();
    initOperationDisplay();
}

void updateDisplay()
{
    if (millis() - _lastDisplayUpdate > DISPLAY_REFRESH_INTERVAL_MS)
    {
        // Show warning display if some limit switch is set to error or triggered
        if(limitSwitchesState.topLimitState == LIMITSTATES::LIMIT_ERROR ||
            limitSwitchesState.topLimitState == LIMITSTATES::LIMIT_TRIGGERED ||
            limitSwitchesState.bottomLimitState == LIMITSTATES::LIMIT_ERROR ||
            limitSwitchesState.bottomLimitState == LIMITSTATES::LIMIT_TRIGGERED) {
            showWarningDisplay();
        } else {
            // Show normal operation display
            showOperationDisplay();
        }

        _lastDisplayUpdate = millis();
    }
}

void showWarningDisplay() 
{
    if(displayState.lastDisplayPage != PAGE_WARNING) {
        lcd.clear();
        displayState.lastDisplayPage = PAGE_WARNING;
        displayState.lastBottomLimitState = LIMITSTATES::LIMIT_OK;
        displayState.lastTopLimitState = LIMITSTATES::LIMIT_OK;
        lcd.setCursor(0, 0);
        lcd.print("Endschalter Fehler");
        lcd.setCursor(0, 1);
        lcd.print("Oben:");
        lcd.setCursor(0, 2);
        lcd.print("Unten:");
    }

    if(displayState.lastTopLimitState != limitSwitchesState.topLimitState) {
        displayState.lastTopLimitState = limitSwitchesState.topLimitState;
        lcd.setCursor(displaySecondColumnOffset, 1);
        switch (limitSwitchesState.topLimitState)
        {
        case LIMITSTATES::LIMIT_ERROR:
            lcd.print("Error");
            lcd.print("    ");
            lcd.setCursor(0, 3);
            lcd.print("Drueck OK um weiter");
            break;
        case LIMITSTATES::LIMIT_TRIGGERED:
            lcd.print("Ausgeloest");
            lcd.setCursor(0, 3);
            lcd.print("Drueck OK um weiter");
            break;
        default:
            lcd.print("           ");
            lcd.setCursor(0, 3);
            lcd.print("                    ");
            break;
        }
    }

    if(displayState.lastBottomLimitState != limitSwitchesState.bottomLimitState) {
        displayState.lastBottomLimitState = limitSwitchesState.bottomLimitState;
        lcd.setCursor(displaySecondColumnOffset, 2);
        switch (limitSwitchesState.bottomLimitState)
        {
        case LIMITSTATES::LIMIT_ERROR:
            lcd.print("Error");
            lcd.print("    ");
            lcd.setCursor(0, 3);
            lcd.print("Drueck OK um weiter");
            break;
        case LIMITSTATES::LIMIT_TRIGGERED:
            lcd.print("Ausgeloest");
            lcd.setCursor(0, 3);
            lcd.print("Drueck OK um weiter");
            break;
        default:
            lcd.print("           ");
            lcd.setCursor(0, 3);
            lcd.print("                    ");
            break;
        }

    }


}

void initOperationDisplay() 
{
    displayState.lastMovingMode = MOVINGSTATES::NORMAL;
    displayState.lastState = STATES::NONE;
    displayState.lastTargetPosition = 0.0;
    displayState.lastCurrentPosition = 0.0;
    
    lcd.setCursor(1, 0);
    lcd.print("Modus:");
    lcd.setCursor(displaySecondColumnOffset, 0);
    lcd.print("normal");

    lcd.setCursor(0, 1);
    lcd.print("Status:");
    lcd.setCursor(displaySecondColumnOffset, 1);
    lcd.print("warte");

    lcd.setCursor(3, 2);
    lcd.print("Ist:");
    lcd.setCursor(displaySecondColumnOffset, 2);
    lcd.print("0.0");
    lcd.setCursor(18, 2);
    lcd.print("mm");

    lcd.setCursor(2, 3);
    lcd.print("Soll:");
    lcd.setCursor(displaySecondColumnOffset, 3);
    lcd.print("0.0");
    lcd.setCursor(18, 3);
    lcd.print("mm");
}

void showOperationDisplay() 
{
    if(displayState.lastDisplayPage != PAGE_OPERATION) {
        lcd.clear();
        displayState.lastDisplayPage = PAGE_OPERATION;
        initOperationDisplay();
    }

    if(displayState.lastMovingMode != currentMovingMode) {
        displayState.lastMovingMode = currentMovingMode;
        lcd.setCursor(displaySecondColumnOffset, 0);
        switch (currentMovingMode)
        {
        case MOVINGSTATES::NORMAL:
            lcd.print("Normal");
            break;
        case MOVINGSTATES::DIRECT:
            lcd.print("Direkt");
            break;
        }
    }

    // Update state if necessary
    if (displayState.lastState != currentMachineState)
    {
        displayState.lastState = currentMachineState;
        lcd.setCursor(displaySecondColumnOffset, 1);
        uint16_t sizeWritten = 0;
        switch (currentMachineState)
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
        case STATES::SPINDLE_ON:
            sizeWritten = lcd.print("Motor");
            break;
        default:
            sizeWritten = lcd.print("Dunno");
            break;
        }
        lcd.setCursor(displaySecondColumnOffset + sizeWritten, 1);
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

    // Update spindle speed if neccessary
    if(currentMachineState == STATES::SPINDLE_ON && displayState.lastSpindleSpeed != currentSpindleSpeed) {
        displayState.lastSpindleSpeed = currentSpindleSpeed;
        int mappedSpeed = map(currentSpindleSpeed, 0,255,0,100);
        lcd.setCursor(16, 1);
        lcd.print(mappedSpeed,1);
        lcd.print("%  ");
    }

    // Update target if necessary
    if (displayState.lastTargetPosition != targetPosition_steps)
    {
        displayState.lastTargetPosition = targetPosition_steps;
        lcd.setCursor(displaySecondColumnOffset, 3);
        uint16_t size = lcd.print(convertStepsToMM(targetPosition_steps), 1);
        lcd.setCursor(displaySecondColumnOffset + size, 3);
        lcd.print("  ");
    }

    // Update currentPos if necessary
    if (displayState.lastCurrentPosition != stepper.currentPosition())
    {
        displayState.lastCurrentPosition = stepper.currentPosition();
        lcd.setCursor(displaySecondColumnOffset, 2);
        uint16_t size = lcd.print(convertStepsToMM(stepper.currentPosition()), 1);
        lcd.setCursor(displaySecondColumnOffset + size, 2);
        lcd.print("  ");
    }
}

float getStepsPerMM()
{
    return (float)MOTOR_STEPS_PER_CYCLE / SPINDLE_PITCH;
}

void performButtonActions()
{
    // ********************
    // OK BUTTON
    // ********************
    // Short press handling
    if (buttonOk.rose())
    {
        if (buttonOk.previousDuration() < 1000)
        {
            // Stop the moving progess if button is pressed again
            if (stepper.isRunning())
            {
                quickStop();
                // resetState();
            }
            else
            {
                Serial.println("OK BUTTON pressed");
                // If a limit switch is triggered - set it to ignore
                if(limitSwitchesState.topLimitState == LIMITSTATES::LIMIT_TRIGGERED || limitSwitchesState.topLimitState == LIMITSTATES::LIMIT_ERROR) {
                    limitSwitchesState.topLimitState = LIMITSTATES::LIMIT_IGNORE;
                    targetPosition_steps = stepper.currentPosition();
                } else if(limitSwitchesState.bottomLimitState == LIMITSTATES::LIMIT_TRIGGERED || limitSwitchesState.bottomLimitState == LIMITSTATES::LIMIT_ERROR) {
                    limitSwitchesState.bottomLimitState = LIMITSTATES::LIMIT_IGNORE;
                    targetPosition_steps = stepper.currentPosition();
                } else if(currentMovingMode == MOVINGSTATES::NORMAL){ // Normal operation - move to the desired position
                    stepper.moveTo(targetPosition_steps);
                }
            }
        }
    }
    // long press handling - reset postion
    if (buttonOk.read() == LOW)
    {
        if (buttonOk.currentDuration() > 1000)
        {
            targetPosition_steps = 0;
            stepper.setCurrentPosition(0);
            return;
        }
    }

    // Abort here if an endstop is triggered!!
    if(limitSwitchesState.topLimitState == LIMITSTATES::LIMIT_TRIGGERED || limitSwitchesState.bottomLimitState == LIMITSTATES::LIMIT_TRIGGERED) {
        return;
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
            if (currentMachineState == STATES::WZW)
            {
                quickStop();
                // resetState();
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
            if (currentMachineState == STATES::PROBING || currentMachineState == STATES::PROBE_ERROR)
            {
                quickStop();
                // resetState();
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
    if (buttonDirect.fell())
    {
        targetPosition_steps = stepper.currentPosition();
        currentMovingMode = MOVINGSTATES::DIRECT;
    }
    else if (buttonDirect.rose())
    {
        currentMovingMode = MOVINGSTATES::NORMAL;
    }

    // ********************
    // Spindle BUTTON/SWITCH
    // ********************
    if (buttonSpindleStart.fell())
    {
        Serial.println("Spindle turned on");
        digitalWrite(spindleOnOffPin, HIGH);
        currentMachineState = STATES::SPINDLE_ON;
    }
}

void setSpindleSpeed()
{
    int spindleSpeedSetting = analogRead(potiRotationSpeedPin);
    int mappedSpeed = map(spindleSpeedSetting, 0, 4095, 0, 255);
    if(INVERT_SPINDLE_SPEED_POTI) {
        mappedSpeed = 255 - mappedSpeed;
    }
    currentSpindleSpeed = mappedSpeed;
    ledcWrite(0, mappedSpeed);
}

void quickStop()
{
    stepper.setAcceleration(100000);
    stepper.moveTo(stepper.currentPosition());
    stepper.runToPosition();
    stepper.setAcceleration(acceleration);
}

void wzw()
{
    Serial.println("Start WZW");
    currentMachineState = STATES::WZW;
    while (!limitTop.rose())
    {
        limitTop.update();
        stepper.move(1);
        stepper.setSpeed(maxSpeed);
        stepper.runSpeedToPosition();
    }
    quickStop();
    // Move 5mm back to get off the limit switch
    stepper.runToNewPosition(stepper.currentPosition() - convertMMToSteps(5));
    // If direct mode is active. Dont move back to the last target pos
    if(currentMovingMode == MOVINGSTATES::DIRECT) {
        targetPosition_steps = stepper.currentPosition();
    }
}

void probe()
{
    Serial.println("Start Probe");
    currentMachineState = STATES::PROBING;

    // move as long as bottom endstop is reached
    while (!limitBottom.rose())
    {
        limitBottom.update();
        stepper.move(-1);
        stepper.setSpeed(maxSpeed);
        stepper.runSpeedToPosition();
    }
    quickStop();

    // Wait for the probepin to be HIGH
    while (limitProbe.read() == LOW)
    {       
        limitProbe.update();
    }

    // Move against probe limit switch with max speed
    moveAgainstProbeLimit(maxSpeed);
    quickStop();

    // Stop if something went wrong
    if (currentMachineState != STATES::PROBING)
    {
        // If direct mode is active. Dont move back to the last target pos
        if(currentMovingMode == MOVINGSTATES::DIRECT) {
            targetPosition_steps = stepper.currentPosition();
        }
        return;
    }

    // Move a littlebit down
    stepper.runToNewPosition(stepper.currentPosition() - convertMMToSteps(5));
    // Wait for the probepin to be HIGH again
    while (limitProbe.read() == LOW)
    {       
        limitProbe.update();
    }
    

    // Move against probe limit switch with slow speed for better accuracy
    moveAgainstProbeLimit(slowSpeed);
    quickStop();

    // Stop if something went wrong
    if (currentMachineState != STATES::PROBING)
    {
        return;
    }

    // Finally reset the position
    stepper.setCurrentPosition(0);
    // If direct mode is active. Dont move back to the last target pos
    if(currentMovingMode == MOVINGSTATES::DIRECT) {
        targetPosition_steps = stepper.currentPosition();
    }
}

void moveAgainstProbeLimit(uint16_t speed)
{
    // move as long as limitProbe is reached
    while (limitProbe.read() == HIGH)
    {
        limitProbe.update();
        limitTop.update();
        limitBottom.update();

        // If endstops were reached during probing - abort.
        if (limitBottom.rose() || limitTop.rose())
        {
            currentMachineState = STATES::PROBE_ERROR;
            break;
        }

        stepper.move(1);
        stepper.setSpeed(speed);
        stepper.runSpeedToPosition();
    }
    limitProbe.update();
}
