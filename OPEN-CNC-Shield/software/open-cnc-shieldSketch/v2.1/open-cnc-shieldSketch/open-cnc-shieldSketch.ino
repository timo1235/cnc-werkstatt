/**
 * OPEN CNC SHIELD Firmware 2.1
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 * 
 * ***********************    General Notes    *****************************
 * - Uncomment means removing the 2 // in front of #define.
 **/

//===========================================================================
// *************************   Includes    **********************************
//===========================================================================
/**
 * If your IDE complains about missing libraries, open the library manager in 
 * your IDE and search for the library, then install it. If you dont know how,
 * google for "install arduino library"
 **/ 
#include <Arduino.h>
// The CNC-Library should be included with this firmware
#include "cnc-library.h"
// OneWire by Jim Studt and many more... -> https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h> 
// DallasTemperature by Miles Burton and many more... -> https://github.com/milesburton/Arduino-Temperature-Control-Library
#include "DallasTemperature.h"

//===========================================================================
// *************************   Configuration    *****************************
//===========================================================================
// Debugging. If enabled, the sketch will print debug states to the serial monitor
// with 9600 Baudrate
// Default: commented 
// #define DEBUG                         // Uncomment to output debug messages

// This setting controls, whether the open cnc shield tries to communicate with the 
// with wireless adapter over I2C. Uncomment this, if you use the wireless adapter. 
// Be sure that this setting is also set in the sketch of the arduino of wireless adapter.
// IMPORTANT - Has to be commented if there is a wired connection on the sub-d port. For example if the wired extension panel is used
// Default: uncommented
#define I2C_TO_WIRELESS_ADAPTER // Comment to disable communication

//===========================================================================
// **************    Temperature and PWM Fan Settings   *********************
//===========================================================================
// Default speed of the fan in percent. This is also the minimal speed if the
// fan is controlled by temperature
// Default: 50
#define DEFAULT_FAN_SPEED_PERCENT 50

// Controls wether the fan speed is adjusted by the onboard temperature sensor. 
// Set to true if it should be enabled.
// Defualt: uncommented
#define SET_FAN_SPEED_BY_TEMPERATURE // Comment to disable

#ifdef SET_FAN_SPEED_BY_TEMPERATURE
    // Interval between fan speed updates from temperatur sensor in milliseconds
    // Default: 5000
    #define TEMP_SENSOR_INTERVAL_MS 5000

    // The temperature at which the fan runs with the speed specified in 'DEFAULT_FAN_SPEED_PERCENT'
    // Default: 20
    #define TEMPERATURE_MIN_CELSIUS 20

    // The temperature at which the fan runs with 100% speed
    // Default: 40
    #define TEMPERATURE_MAX_CELSIUS 40
#endif

//===========================================================================
// *************************   Autosquare    ********************************
//===========================================================================
// Autosquare can be enabled or disabled with the jumper on the shield itself

// Delay between steps in microseconds if this is set, the poti is ignored
// Default: commented
//#define AS_STEP_DELAY_US 4000
// Delay between pressing the autosquare button and starting autosquare 
// in miliseconds, should be at least 500 
// Default: 1000
#define AS_START_DELAY_MS 1000

// Samples of every autosqare speed poti reading
// No need to touch
// Default: 5
#define AS_POTI_SAMPLES 5

// Minimal delay between steps in microseconds - When the poti is at 0%
// Default: 10
#define AS_POTI_MIN_DELAY_US 10

// Maximum delay between steps in microseconds - When the poti is at 100%
// Default: 5000
#define AS_POTI_MAX_DELAY_US 5000

// State of endstops - Use LOW for normally open endstops and HIGH for normally closed endstops
// Possible values: LOW // HIGH
// Default: LOW
#define AS_ENDSTOP_STATE_TRIGGERED LOW

// Move stepper to the other direction, if autosquare is finished for the axis
// The stepper stops moving, as soon as the endstop is released
// Default: uncommented
#define AS_MOVE_STEPPER_FROM_ENDSTOP // Comment to disable

#ifndef I2C_TO_WIRELESS_ADAPTER
    //===========================================================================
    // *************************   Display    ***********************************
    //===========================================================================
    // Uncomment the next line only, if a display is connected by a wire with the shield.
    // For example if the extension panel is used. If the wireless adapter is connected
    // to the shield the next line has to be commented like this: "//#define USE_DISPLAY"
    // Default: commented
    // #define USE_DISPLAY // comment with "//" if display is not needed

    #ifdef USE_DISPLAY 
        // U8g2 library by Oliver -> https://github.com/olikraus/u8g2
        #include <U8g2lib.h>
        // How often the display should be updated, in miliseconds
        // Default: 1000
        #define DISPLAY_REFRESH_INTERVAL_MS 1000
    
        // The displaylayout that should be used. Be aware of the size of the display
        // Possible values:
        // DISPLAYLAYOUT::Layout_128X32_Autosquare
        // DISPLAYLAYOUT::Layout_128X32_Fan_and_Temp
        // DISPLAYLAYOUT::Layout_128X32_Temp_and_Autosquare
        // DISPLAYLAYOUT::Layout_128X64_1
        // Default: DISPLAYLAYOUT::Layout_128X32_Fan_and_Temp
        #define DISPLAY_LAYOUT DISPLAYLAYOUT::Layout_128X32_Fan_and_Temp

        // We need to choose the right class for the display here.
        // Uncomment only one of them. If you have another display, 
        // take a look at the U8g2 documentation.
        // 128x32 Display
        U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); 
        // 128x64 Display
        // U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
    #endif
#endif

//===========================================================================
// ********** Only edit below, if you know what you are doing ***************
//===========================================================================
#ifdef I2C_TO_WIRELESS_ADAPTER
    // Default Arduino Library
    #include <Wire.h>
#endif

enum AXIS
{
    x,
    y,
    z
};

typedef struct
{
    AXIS axis;
    byte stepPin;
    byte endStopPin;
    byte dirPin;
    byte dirConfigPin;
    AUTOSQUARESTATE state;
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
    {x, A15, 10, A6, dirXRevConfig, off, "X1:"},
    {x, A14, 8, 49, dirXRevConfig, off, "X2:"},
    {y, A13, 7, A5, dirYRevConfig, off, "Y1:"},
    {y, A12, 6, 53, dirYRevConfig, off, "Y2:"},
    {z, A11, 12, 16, dirZRevConfig, off, "Z1:"},
    {z, A10, 11, A7, dirZRevConfig, off, "Z2:"},
};

CNCTemperature externalTemperature(tempSensorPin, 1);
CNCTemperature onboardTemperature(onBoardTempSensorPin, 1);

// Programm variales
uint32_t actualAS_STEP_DELAY_US = 0;
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

byte asAxisStates[] = {/*x:*/ 0, /*y:*/ 0, /*z:*/ 0};

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
            if (digitalRead(stepper->endStopPin) == AS_ENDSTOP_STATE_TRIGGERED)
            {
                stepper->state = done;
                asAxisStates[stepper->axis] ++;
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

    #ifdef AS_MOVE_STEPPER_FROM_ENDSTOP
        // We move the stepper backwards, when endstop is reached and the axis
        // finished autosquare
        for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
        {
            STEPPER *stepper = &steppers[i];
            // Move only if the state is done, endstop is triggered and the whole axis is done
            if (stepper->state == done 
                && digitalRead(stepper->endStopPin) == AS_ENDSTOP_STATE_TRIGGERED
                && asAxisStates[stepper->axis] == 2)
            {
                // Change the direction of the stepper
                digitalWrite(stepper->dirPin, !digitalRead(stepper->dirConfigPin));
                digitalWrite(stepper->stepPin, HIGH);
            }
        }
    #endif

    // Delay before setting to low - this is the speed of the steppers
    delayMicroseconds(actualAS_STEP_DELAY_US);

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
    #ifdef AS_MOVE_STEPPER_FROM_ENDSTOP
        // Reset axis state 
        asAxisStates[0] = 0;
        asAxisStates[1] = 0;
        asAxisStates[2] = 0;
    #endif

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
#ifdef SET_FAN_SPEED_BY_TEMPERATURE
    void setFanSpeedByTemperature() {
        if(millis() - lastSettingFanSpeed_MS > TEMP_SENSOR_INTERVAL_MS) {
            float actualOnboardTemperature = onboardTemperature.getTemperature();
            uint8_t defaultFanSpeedAbsolute = map(DEFAULT_FAN_SPEED_PERCENT, 0, 100, 0, 255);  
 
            if(actualOnboardTemperature < TEMPERATURE_MIN_CELSIUS) lastDisplayFanSpeed = defaultFanSpeedAbsolute;
            else if(actualOnboardTemperature > TEMPERATURE_MAX_CELSIUS) lastDisplayFanSpeed = 255;
            else lastDisplayFanSpeed = map(actualOnboardTemperature, TEMPERATURE_MIN_CELSIUS, TEMPERATURE_MAX_CELSIUS, defaultFanSpeedAbsolute, 255);

            analogWrite(fanPwm, lastDisplayFanSpeed);
            lastSettingFanSpeed_MS = millis();
        }  
    }
#endif

#include "cnclibrary_common_functions.h"

void setup()
{
    #ifdef DEBUG
        Serial.begin(9600);
    #endif

    externalTemperature.setup();
    onboardTemperature.setup();

    #ifdef USE_DISPLAY
        displaySetup();
    #endif

    initialiseInputsOutputs();
    initPWMFan();
    checkAutosquareEnabledForEachAxis();

    #ifdef I2C_TO_WIRELESS_ADAPTER
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
        if(millis() - timeStartButtonWasPressed > AS_START_DELAY_MS) {
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
        // terminate autosquare if it is initialized
        if (autosuareInitialised) {
            terminate();
        }

        externalTemperature.loop();   
        onboardTemperature.loop();     
        #ifdef SET_FAN_SPEED_BY_TEMPERATURE
            setFanSpeedByTemperature();
        #endif

        #ifdef USE_DISPLAY
            updateDataForDisplay();        
            displayUpdate(DISPLAY_LAYOUT);
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
    analogWrite(fanPwm, DEFAULT_FAN_SPEED_PERCENT);
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

#ifdef I2C_TO_WIRELESS_ADAPTER
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
    #ifdef AS_STEP_DELAY_US
        actualAS_STEP_DELAY_US = AS_STEP_DELAY_US;
    #endif
    #ifndef AS_STEP_DELAY_US
        uint16_t sum = 0;
        for (int i = 0; i < AS_POTI_SAMPLES; i++)
        {
            sum+= analogRead(autosquareSpeed);
            delayMicroseconds(500);
        }
        actualAS_STEP_DELAY_US = map(sum / AS_POTI_SAMPLES, 0, 1023, AS_POTI_MAX_DELAY_US, AS_POTI_MIN_DELAY_US);
    #endif
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
