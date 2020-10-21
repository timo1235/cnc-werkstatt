/*
    OPEN-CNC-Shield Firmware 2.0
    find out more: blog.altholtmann.com

    by Timo Altholtmann
*/

#include <Arduino.h>
#include "cnc-library.h"

/**
 * Configuration
 */
// #define DEBUG // Uncomment to output debug messages

/**
 * USE_DISPLAY
 * Uncomment the next line only, if a display is connected by a wire with the shield. For example if the extension panel is used.
 * If the wireless solution is connected to shield the next line has to be commented like this: "//#define USE_DISPLAY"
 */
//#define USE_DISPLAY // comment with "//" if display is not needed
#ifdef USE_DISPLAY 
    #include <U8g2lib.h>
    uint32_t displayRefreshInterval_MS = 1000;      // The display is refreshed every x milliseconds
    DISPLAYLAYOUT displayLayout = DISPLAYLAYOUT::Layout_128X32_Autosquare;
    // Display byte SDA = A4, SCL = A5
    // 128x32 Display
    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); 
    // 128x64 Display
    // U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#endif

/**
 * WIRELESS ADAPTER CONNECTED
 * Comment with // if wireless adapter is not connected!
 * IMPORTANT - Has to be commented if there is a wired connection on the sub-d port. For example if the wired extension panel is used
 */
//#define WIRELESS_ADAPTER_CONNECTED

// Autosquare settings - On / Off setting is done with the jumper configuration
const uint16_t stepDelay = 4000;            // delay between steps in microseconds if no poti is connected - set to 0 to use a poti for setting speed
const uint16_t startDelay = 1000;           // delay between pressing the autosquare button and starting autosquare - in miliseconds, should be at least 500 
const uint8_t speedPotiSamples = 5;         // Samples of every autosqare speed poti reading
const uint16_t speedPotiMin = 10;           // min delay between steps in microseconds
const uint16_t speedPotiMax = 5000;         // max delay between steps in microseconds
const uint8_t endstopStateTriggered = LOW;  // Use LOW for normally open endstops and HIGH for normally closed endstops

// PWM Fan
const uint8_t defaultFanSpeed = 50;                 // default speed of the fan in percent. This is also the minimal speed if the fan is controlled by temperature
const bool shouldSetFanSpeedByTemperature = true;   // Controls wether the fan speed is adjusted by the onboard temperature sensor

// Temperature sensors
const uint16_t tempSensorUpdateInterval = 5000; // interval between temperatur sensor reads in milliseconds
const int16_t temperatureMin = 20;              // the temperatur at which the fan runs with the speed specified in 'defaultFanSpeed'
const int16_t temperatureMax = 40;              // the temperatur at which the fan runs with 100% speed

// ------------------------------------------------------------------------------
// Only edit below if you know what you are doing
// ------------------------------------------------------------------------------
#ifdef WIRELESS_ADAPTER_CONNECTED
    #include <Wire.h>
#endif

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
const byte tempSensorPin = 24;
const byte onBoardTempSensorPin = 46;
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

CNCTemperature externalTemperature(tempSensorPin, 1);
CNCTemperature onboardTemperature(onBoardTempSensorPin, 1);

// Programm variales
uint32_t actualStepDelay = 0;
byte autosuareInitialised = false;
uint64_t timeStartButtonWasPressed = 0;
DATA_FROM_CONTROLLER dataFromController = {
    5,
    5,
    5,
    AUTOSQUARESTATE::undefined,
    AUTOSQUARESTATE::undefined,
    AUTOSQUARESTATE::undefined,
    AUTOSQUARESTATE::undefined,
    AUTOSQUARESTATE::undefined,
    AUTOSQUARESTATE::undefined,
};
DATA_FOR_DISPLAY dataForDisplay;

uint32_t lastDebugOutput_MS = 0;
uint32_t lastSettingFanSpeed_MS = 0;
byte lastDisplayFanSpeed;


void checkAutosquareEnabledForEachAxis();
void initPWMFan();
void initialiseInputsOutputs();
void initialiseI2c();
void answerI2cCommunication();
void requestEvent();
void updateAutosquareDelay();
void updateDataForDisplay();

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
                #ifdef DEBUG
                    Serial.print("AS ");
                    Serial.print(stepper->label);
                    Serial.println("done");
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
    #ifdef DEBUG
        Serial.println("Initializing");
    #endif
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

    autosuareInitialised = true;
}

void terminate()
{
    #ifdef DEBUG
        Serial.println("Terminating");
    #endif

    digitalWrite(asStop, LOW);
    pinMode(in9Mega, INPUT);
    autosuareInitialised = false;
}

void setFanSpeedByTemperature() {
    if(shouldSetFanSpeedByTemperature && millis() - lastSettingFanSpeed_MS > 5000) {
        float actualOnboardTemperature = onboardTemperature.getTemperature();
        uint8_t defaultFanSpeedAbsolute = map(defaultFanSpeed, 0, 100, 0, 255);   

        if(actualOnboardTemperature < temperatureMin) lastDisplayFanSpeed = defaultFanSpeedAbsolute;
        else if(actualOnboardTemperature > temperatureMax) lastDisplayFanSpeed = 255;
        else lastDisplayFanSpeed = map(actualOnboardTemperature, temperatureMin, temperatureMax, defaultFanSpeedAbsolute, 255);

        analogWrite(fanPwm, lastDisplayFanSpeed);
        lastSettingFanSpeed_MS = millis();
    }  
}

#include "cnclibrary_common_functions.h"

void setup()
{
    Serial.begin(9600);
    externalTemperature.setup();
    onboardTemperature.setup();

    #ifdef USE_DISPLAY
        displaySetup();
    #endif

    initialiseInputsOutputs();

    initPWMFan();
    checkAutosquareEnabledForEachAxis();
    #ifdef WIRELESS_ADAPTER_CONNECTED
        initialiseI2c();
    #endif

    #ifdef DEBUG
        Serial.println("Setup done");
    #endif
}

void loop()
{   
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
        }
    } else {
        // reset timeStartButtonWasPressed
        timeStartButtonWasPressed = 0;
        // terminate if initialized
        if (autosuareInitialised) {
            terminate();
        }

        externalTemperature.loop();   
        onboardTemperature.loop();     
    
        setFanSpeedByTemperature();

        #ifdef USE_DISPLAY
            updateDataForDisplay();        
            displayUpdate(displayLayout);
        #endif

        #ifdef DEBUG
            if (millis() - lastDebugOutput_MS > 10000) {
                Serial.print("External Temp: ");
                Serial.println(externalTemperature.getTemperature());

                Serial.print("Onboard Temp: ");
                Serial.print(onboardTemperature.getTemperature());
                Serial.print(" C, Fan Speed: ");
                Serial.print(lastDisplayFanSpeed);
                Serial.println(" of 255");  

                lastDebugOutput_MS = millis();
            }
        #endif
    }    
}

/**
 * Checks the jumper configuration for each axis and initialises the state variables 
*/
void checkAutosquareEnabledForEachAxis() { 
    #ifdef DEBUG
        Serial.println("Reading Axis Config");  
    #endif
    if (digitalRead(asXOn) == LOW)
    {
        #ifdef DEBUG
            Serial.println("AS X = ON");
        #endif
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
        #ifdef DEBUG
            Serial.println("AS Y = ON");
        #endif
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
        #ifdef DEBUG
            Serial.println("AS Z = ON");
        #endif
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
#ifdef WIRELESS_ADAPTER_CONNECTED
    /**
     *  Try to communicate to Wireless Adapter via i2c
     **/ 
    void initialiseI2c() {
        Wire.begin(10);                // join i2c bus with address #8
        Wire.onRequest(answerI2cCommunication); 
    }
    void answerI2cCommunication() {
        dataFromController.onboardTemperature = onboardTemperature.getTemperature();
        dataFromController.externalTemperature = externalTemperature.getTemperature();
        dataFromController.fanSpeed = lastDisplayFanSpeed;
        dataFromController.x1 = steppers[0].state;
        dataFromController.x2 = steppers[1].state;
        dataFromController.y1 = steppers[2].state;
        dataFromController.y2 = steppers[3].state;
        dataFromController.z1 = steppers[4].state;
        dataFromController.z2 = steppers[5].state;
        Wire.write((byte*)&dataFromController, sizeof(DATA_FROM_CONTROLLER));  
    }
#endif

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
    void updateDataForDisplay() {
        dataForDisplay.onboardTemperature = onboardTemperature.getTemperature();
        dataForDisplay.externalTemperature = externalTemperature.getTemperature();
        dataForDisplay.stateX1 = steppers[0].state;
        dataForDisplay.stateX2 = steppers[1].state;
        dataForDisplay.stateY1 = steppers[2].state;
        dataForDisplay.stateY2 = steppers[3].state;
        dataForDisplay.stateZ1 = steppers[4].state;
        dataForDisplay.stateZ2 = steppers[5].state;
        dataForDisplay.fanSpeed = map(lastDisplayFanSpeed, 100, 255, 1, 7);
    }
#endif
