/*
    OPEN-CNC-Shield Firmware 1.0
    find out more: blog.altholtmann.com

    by Timo Altholtmann
*/

#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>


// Programm configuration
const uint16_t stepDelay = 4000; // delay between steps in microseconds if no poti is connected - set to 0 to use a poti for setting speed
const uint16_t startDelay = 1000; // delay between pressing the autosquare button and starting autosquare - in miliseconds, should be at least 500 
const uint8_t speedPotiSamples = 10; // Samples of every speed poti reading
const uint16_t speedPotiMin = 10;  // min delay between steps in microseconds
const uint16_t speedPotiMax = 5000; // max delay between steps in microseconds
const uint16_t tempSensorUpdateInterval = 5000; // interval between temperatur sensor reads in milliseconds
const uint8_t endstopStateTriggered = HIGH; // Use HIGH for normally open endstops and LOW for normally closed endstops
const uint8_t defaultFanSpeed = 50; // default speed of the fan in percent. This is also the minimal speed.

#define DEBUG ;                         // Uncomment to output debug messages
// #define READ_TEMPERATURES // comment with "//" if temperature sensor is not needed
#define ONBOARD_TEMPERATURE_FAN_CONTROL // comment with "//" if temperature sensor is not needed
#ifdef ONBOARD_TEMPERATURE_FAN_CONTROL
    int16_t temperatureMin = 20;   // the temperature to start the fan in celsius
    int16_t temperatureMax = 40;   // the maximum temperature when fan is at 100%
#endif

//#define USE_DISPLAY // comment with "//"

// ------------------------------------------------------------------------------
// Only edit below if you know what you are doing
// ------------------------------------------------------------------------------

#ifdef USE_DISPLAY 
    #include <U8x8lib.h>
#endif

#if defined(READ_TEMPERATURES) || defined(ONBOARD_TEMPERATURE_FAN_CONTROL)
    #include <OneWire.h> 
    #include "DallasTemperature.h"
#endif

enum AUTOSQUARESTATE
{
    undefined,
    done,
    off
};

enum AXIS
{
    x,
    y,
    z
};

enum i2cState
{
    unset,
    established,
    failure
};

typedef struct
{
    AXIS axis;
    byte stepPin;
    byte endStopPin;
    byte dirPin;
    byte dirConfigPin;
    AUTOSQUARESTATE state;
    byte stateDisplayRow;
    byte stateDisplayColumn;
    char label[4];
} STEPPER;

// Pin configuration
// Outs
const byte in9Mega = 5;
const byte asStop = 39;

const byte out1 = 40;
const byte out2 = 41;
const byte out3 = 42;
const byte out4 = 43;
const byte out5 = 45;
const byte out6 = 44;
const byte out7 = 30;
const byte out8 = 31;
// INs
const byte tempSensor = 24;
const byte onBoardTempSensor = 46;
const byte autosquareButton = 23;
const byte autosquareSpeed = A2;
const byte in1 = 15;
const byte in2 = 14;
const byte in10 = 4;
const byte in11 = 3;
const byte in12 = 2;
const byte rotationSpeed = A4;
const byte feedrate = A3;
// INs - user configuartion
const byte asXOn = 34;
const byte asYOn = 33;
const byte asZOn = 32;
const byte dirXRevConfig = 37;
const byte dirYRevConfig = 36;
const byte dirZRevConfig = 35;
// unused
const byte spindelPwm = 48;
const byte fanPwm = 9;
const byte spindelOnOff = 26;
const byte programmStart = 27;
const byte motorStart = 47;

STEPPER steppers[] = {
    {x, A15, 10, A6, dirXRevConfig, off, 0, 0, "X1:"},
    {x, A14, 8, 49, dirXRevConfig, off, 0, 8, "X2:"},
    {y, A13, 7, A5, dirYRevConfig, off, 1, 0, "Y1:"},
    {y, A12, 6, 53, dirYRevConfig, off, 1, 8, "Y2:"},
    {z, A11, 12, 16, dirZRevConfig, off, 2, 0, "Z1:"},
    {z, A10, 11, A7, dirZRevConfig, off, 2, 8, "Z2:"},
};

#ifdef USE_DISPLAY 
    // Display byte SDA = A4, SCL = A5
    U8X8_SSD1306_128X32_UNIVISION_SW_I2C display(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
    // Hardware i2c does not work for me with long wires
    // U8X8_SSD1306_128X32_UNIVISION_HW_I2C display(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
    const uint8_t *defaultFont = u8x8_font_pxplustandynewtv_f;
#endif
#ifdef READ_TEMPERATURES 
    OneWire oneWire(tempSensor); 
    DallasTemperature sensors(&oneWire);
    uint64_t lastReadTemperature = 0;
#endif
#ifdef ONBOARD_TEMPERATURE_FAN_CONTROL
    OneWire onboardWire(onBoardTempSensor); 
    DallasTemperature onboardSensors(&onboardWire);
    uint64_t lastOnboardTemperatureReading = 0;
    uint8_t lastFanSpeed = defaultFanSpeed;
#endif

// Programm variales
uint32_t actualStepDelay = 0;
byte autosuareInitialised = false;
uint64_t timeStartButtonWasPressed = 0;
AUTOSQUARESTATE lastDisplayStates[sizeof(steppers) / sizeof(STEPPER)];
i2cState wirelessAdapterCommunicationState = unset;
uint32_t timeArduinoStarted = 0;


void checkAutosquareEnabledForEachAxis();
void initPWMFan();
void initialiseInputsOutputs();
void initialiseI2c();
void answerI2cCommunication();


void updateAutosquareDelay() {
    // Dont read poti if a fixed speed is set
    if(stepDelay != 0 && actualStepDelay != stepDelay) {
        actualStepDelay = stepDelay;
        return;
    }

    uint16_t sum = 0;
    for (int i = 0; i < speedPotiSamples; i++)
    {
        sum+= analogRead(autosquareSpeed);
        delayMicroseconds(500);
    }
    actualStepDelay = map(sum / speedPotiSamples, 0, 1023, speedPotiMax, speedPotiMin); 
}
#ifdef USE_DISPLAY 
    void setDisplayStateOfStepper(STEPPER *stepper, byte displayStatePosition) {
        // Set last display state
        lastDisplayStates[displayStatePosition] = stepper->state;
        byte columPosition = stepper->stateDisplayColumn + (sizeof(stepper->label)/sizeof(char));

        // Empty last state
        display.setFont(defaultFont);
        display.drawString(columPosition, stepper->stateDisplayRow, "   ");

        if(stepper->state == off) {
            display.setFont(defaultFont);
            display.drawString(columPosition, stepper->stateDisplayRow, "off");
        } else if(stepper->state == undefined){    
            display.setFont(defaultFont);        
            display.drawString(columPosition, stepper->stateDisplayRow, "?");
        } else if(stepper->state == done) {
            display.setFont(u8x8_font_open_iconic_check_1x1);
            display.drawGlyph(columPosition, stepper->stateDisplayRow, 64);
        }
    }
    void updateDisplay()
    {
        for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
        {
            STEPPER *stepper = &steppers[i];
            // Only update when things changed
            if(stepper->state == lastDisplayStates[i]) {
                continue;
            }  

            setDisplayStateOfStepper(stepper, i);      
        }
    }

    void initDisplay() {
        display.begin();
        // display.setBusClock(100000);
        // Stepper states
        for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
        {
            STEPPER *stepper = &steppers[i];
            display.setFont(defaultFont);
            display.drawString(stepper->stateDisplayColumn, stepper->stateDisplayRow, stepper->label);
            setDisplayStateOfStepper(stepper, i); 
        }   
    }
#endif

void autosquare()
{
    // Set HIGH Pulse if state is on and endstop not pressed
    for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
    {
        STEPPER *stepper = &steppers[i];
        // Continue for loop if stepper is set to off or done
        if (stepper->state == off || stepper->state == done)
        {
            continue;
        }
        // Check if state is undefined
        if (stepper->state == undefined)
        {
            // Set state to ok if endstop is reached
            if (digitalRead(stepper->endStopPin) == endstopStateTriggered)
            {
                stepper->state = done;
                #ifdef USE_DISPLAY  
                  updateDisplay();
                #endif               
            }            
            // Set HIGH pulse if endstop not reached
            else
            {
                digitalWrite(stepper->stepPin, HIGH);
            }
        }
    }
    // Delay before setting to low - this is the speed of the steppers
    delayMicroseconds(actualStepDelay);

    // Set stepper steps to LOW if they are HIGH
     for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
    {
        STEPPER *stepper = &steppers[i]; 
        if(digitalRead(stepper->stepPin) == HIGH) {
            digitalWrite(stepper->stepPin, LOW);
        }
    }
}

void initialize()
{
    Serial.println("Initializing");
    checkAutosquareEnabledForEachAxis();    
    bool allDone = true;
    for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
    {        
        // Check if all stepper are already done
        if(steppers[i].state == undefined) {
            allDone = false;
        }

        // Read config and set stepper direction accordingly
        digitalWrite(steppers[i].dirPin, digitalRead(steppers[i].dirConfigPin));
    }

    // Take control over steppers
    digitalWrite(asStop, HIGH);
    // Trigger IN9 to estlcam arduino mega
    pinMode(in9Mega, OUTPUT);
    digitalWrite(in9Mega, LOW);

    // Reset stepper states if all are done
    for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
    {
        if(allDone && steppers[i].state == done) {
            steppers[i].state = undefined;
        }
    }

    // Set pulse speed
    updateAutosquareDelay();
    
    #ifdef USE_DISPLAY  
      updateDisplay();
    #endif

    autosuareInitialised = true;
}

void terminate()
{
    Serial.println("Terminating");

    digitalWrite(asStop, LOW);
    pinMode(in9Mega, INPUT);
    autosuareInitialised = false;
}

#ifdef READ_TEMPERATURES 
    void updateTemperatureSensor() {
            sensors.requestTemperatures();
            Serial.println(sensors.getTempCByIndex(0));
            display.setFont(defaultFont);
            display.drawUTF8(0,3, "Temp:");
            display.setCursor(6,3);
            display.print(sensors.getTempCByIndex(0));
            display.drawString(10,3,"Grad");
            lastReadTemperature = millis();
        
    }
#endif   

#ifdef ONBOARD_TEMPERATURE_FAN_CONTROL 
    void readOnboardTemperatureAndSetFanSpeed() {
        onboardSensors.requestTemperatures();   
        int8_t actualTemperature = onboardSensors.getTempCByIndex(0);  
        uint8_t defaultFanSpeedAbsolute = map(defaultFanSpeed, 0, 100, 0, 255);   

        if(actualTemperature < temperatureMin) lastFanSpeed = defaultFanSpeedAbsolute;
        else if(actualTemperature > temperatureMax) lastFanSpeed = 255;
        else lastFanSpeed = map(actualTemperature, temperatureMin, temperatureMax, defaultFanSpeedAbsolute, 255);

        analogWrite(fanPwm, lastFanSpeed);
        lastOnboardTemperatureReading = millis();

        #ifdef DEBUG
            Serial.print("OnBoard Temp: ");
            Serial.print(onboardSensors.getTempCByIndex(0));
            Serial.print(" C, Fan Speed: ");
            Serial.print(lastFanSpeed);
            Serial.println(" of 255");
            
        #endif
    }
#endif

void setup()
{
    Serial.begin(9600);

    initialiseInputsOutputs();

    initPWMFan();

    #ifdef READ_TEMPERATURES 
        // Inititalise temperature sensors
        sensors.begin();
        updateTemperatureSensor();
    #endif

    #ifdef ONBOARD_TEMPERATURE_FAN_CONTROL
        onboardSensors.begin();
        readOnboardTemperatureAndSetFanSpeed();
    #endif

    checkAutosquareEnabledForEachAxis();
}

void loop()
{
    #ifdef ONBOARD_TEMPERATURE_FAN_CONTROL 
        if(millis() - lastOnboardTemperatureReading > 10000) {
            readOnboardTemperatureAndSetFanSpeed();
        }
    #endif

    // Check if button is pressed
    if (digitalRead(autosquareButton) == LOW) {
        // Check if start button was just pressed
        if(timeStartButtonWasPressed == 0) {
            timeStartButtonWasPressed = millis();
        }

        // Check if button was pressed long enough
        if(millis() - timeStartButtonWasPressed > startDelay) {
            // Check if we have to initinalize
            if (!autosuareInitialised) {
                initialize();
                delay(10);
            }            
            autosquare();
        } else {
            // Use waiting time to determine if 
        }
    } else {
        // reset timeStartButtonWasPressed
        timeStartButtonWasPressed = 0;
        // terminate if initialized
        if (autosuareInitialised) {
            terminate();
        }
        #ifdef READ_TEMPERATURES 
            if(millis() - lastReadTemperature > tempSensorUpdateInterval) {
                updateTemperatureSensor();
            }
        #endif
        #ifdef ONBOARD_TEMPERATURE_FAN_CONTROL 
            if(millis() - lastOnboardTemperatureReading > 10000) {
                readOnboardTemperatureAndSetFanSpeed();
            }
        #endif
    } 
}

/**
 * Checks the jumper configuration for each axis and initialises the state variables 
*/
void checkAutosquareEnabledForEachAxis() { 
    Serial.println("Reading Axis Config");   
    if (digitalRead(asXOn) == LOW)
    {
        for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
        {
            if (steppers[i].axis == x)
            {
                steppers[i].state = undefined;
            }
        }
    }
    if (digitalRead(asYOn) == LOW)
    {
        for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
        {
            if (steppers[i].axis == y)
            {
                steppers[i].state = undefined;
            }
        }
    }
    if (digitalRead(asZOn) == LOW)
    {
        for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
        {
            if (steppers[i].axis == z)
            {
                steppers[i].state = undefined;
            }
        }
    }
}
/**
 *  Sets the interal timer to pwm fan frequency 
 **/
void initPWMFan() {
    // PWM FAN Setup
    //Timer 1: Prescaler 0
    TCCR1B = ((TCCR1B & B11111000) | B00000001);
    analogWrite(fanPwm, defaultFanSpeed);
}
/**
 * Sets the pinmode for all inputs and outputs
 **/
void initialiseInputsOutputs() {
    // Endstops and steppers
    for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
    {
        pinMode(steppers[i].endStopPin, INPUT_PULLUP);
        pinMode(steppers[i].stepPin, OUTPUT);
        pinMode(steppers[i].dirPin, OUTPUT);
    }

    // Inputs
    pinMode(autosquareButton, INPUT_PULLUP);
    pinMode(asXOn, INPUT_PULLUP);
    pinMode(asYOn, INPUT_PULLUP);
    pinMode(asZOn, INPUT_PULLUP);
    pinMode(dirXRevConfig, INPUT_PULLUP);
    pinMode(dirYRevConfig, INPUT_PULLUP);
    pinMode(dirZRevConfig, INPUT_PULLUP);
    pinMode(in1, INPUT_PULLUP);
    pinMode(in2, INPUT_PULLUP);
    pinMode(in10, INPUT_PULLUP);
    pinMode(in11, INPUT_PULLUP);
    pinMode(in12, INPUT_PULLUP);

    // Outputs
    pinMode(asStop, OUTPUT);
    digitalWrite(asStop, LOW);    
    pinMode(fanPwm, OUTPUT);

    // unused pins, set as input to not interfer with estlcam
    pinMode(out1, INPUT);
    pinMode(out2, INPUT);
    pinMode(out3, INPUT);
    pinMode(out4, INPUT);
    pinMode(out5, INPUT);
    pinMode(out6, INPUT);
    pinMode(out7, INPUT);
    pinMode(out8, INPUT);
    pinMode(in9Mega, INPUT);
    pinMode(spindelPwm, INPUT);    
    pinMode(spindelOnOff, INPUT);
    pinMode(programmStart, INPUT);
    pinMode(motorStart, INPUT);
}
