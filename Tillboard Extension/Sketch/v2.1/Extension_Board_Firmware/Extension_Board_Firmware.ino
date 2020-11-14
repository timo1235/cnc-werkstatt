/**
 * Tillboard Extension Firmware 2.1
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 * 
 * ***********************    General Notes    *****************************
 * - Uncomment means removing the 2 // in front of #define.
 * 
 * ***********************    Changelog    *********************************
 * #2.1
 * - Changed configuration variables to #define
 * 
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

//===========================================================================
// *************************   Configuration    *****************************
//===========================================================================
// This setting controls, whether the open cnc shield tries to communicate with the 
// with wireless adapter over I2C. Uncomment this, if you use the wireless adapter. 
// Be sure that this setting is also set in the sketch of the arduino of wireless adapter.
// IMPORTANT - Has to be commented if there is a wired connection on the sub-d port. For example if the wired extension panel is used
// Default: uncommented
#define I2C_TO_WIRELESS_ADAPTER // Comment to disable communication

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

//===========================================================================
// **************    Temperature Settings   *********************************
//===========================================================================
// Reads the temperature of an external connected temperature sensor
// Default: uncommented
#define READ_TEMPERATURES // comment with "//" if temperature sensor is not needed

#ifdef READ_TEMPERATURES
    // Interval between temperatur sensor reads in milliseconds
    // Default: 5000
    #define TEMP_SENSOR_UPDATE_INTERVAL 5000
#endif

#ifndef I2C_TO_WIRELESS_ADAPTER
    //===========================================================================
    // *************************   Display    ***********************************
    //===========================================================================
    // Uncomment the next line only, if a display is connected by a wire with the shield.
    // For example if the extension panel is used. If the wireless adapter is connected
    // to the shield the next line has to be commented like this: "//#define USE_DISPLAY"
    // Default: commented
    #define USE_DISPLAY // comment with "//" if display is not needed

    #ifdef USE_DISPLAY 
        // U8g2 library by Oliver -> https://github.com/olikraus/u8g2
        #include <U8g2lib.h>

        // Display byte SDA = A4, SCL = A5
        U8X8_SSD1306_128X32_UNIVISION_SW_I2C display(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
        // Hardware i2c does not work for me with long wires
        // U8X8_SSD1306_128X32_UNIVISION_HW_I2C display(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
        const uint8_t *defaultFont = u8x8_font_pxplustandynewtv_f;
    #endif
#endif

// ------------------------------------------------------------------------------
// Only edit below if you know what you are doing
// ------------------------------------------------------------------------------
#ifdef I2C_TO_WIRELESS_ADAPTER
    // Default Arduino Library
    #include <Wire.h>
#endif

#ifdef READ_TEMPERATURES
    // OneWire by Jim Studt and many more... -> https://www.pjrc.com/teensy/td_libs_OneWire.html
    #include <OneWire.h> 
    // DallasTemperature by Miles Burton and many more... -> https://github.com/milesburton/Arduino-Temperature-Control-Library
    #include "DallasTemperature.h"
#endif

enum AXIS
{
    x,
    y
};

typedef struct
{
    AXIS axis;
    byte stepPin;
    byte endStopPin;
    AUTOSQUARESTATE state;
    byte stateDisplayRow;
    byte stateDisplayColumn;
    char label[4];
} STEPPER;

STEPPER steppers[] = {
    {x, 11, 5, off, 0, 0, "X1:"},
    {x, 3, 6, off, 0, 8, "X2:"},
    {y, 12, 7, off, 1, 0, "Y1:"},
    {y, 4, 8, off, 1, 8, "Y2:"},
};

// Pin configuration
// Outs
byte in9Mega = A0;
byte asStop = 2;
// INs
byte tempSensor = 9;
byte autosquareButton = 10;
byte autosquareSpeed = A1;
// INs - user configuartion
byte asXOn = A3;
byte asYOn = A2;

#ifdef READ_TEMPERATURES 
    OneWire oneWire(tempSensor); 
    DallasTemperature sensors(&oneWire);
#endif


// Programm variales
uint32_t actualAS_STEP_DELAY_US = 0;
byte autosuareInitialised = false;
uint64_t timeStartButtonWasPressed = 0;
AUTOSQUARESTATE lastDisplayStates[sizeof(steppers) / sizeof(STEPPER)];
uint64_t lastReadTemperature = 0;
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
            if (digitalRead(stepper->endStopPin) == AS_ENDSTOP_STATE_TRIGGERED)
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
    Serial.println("Initializing");
    // Check if all stepper are already done
    bool allDone = true;
    for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
    {
        if(steppers[i].state == undefined) {
            allDone = false;
        }
    }

    // Take control over steppers
    digitalWrite(asStop, HIGH);
    // Trigger IN9 to arduino mega
    digitalWrite(in9Mega, LOW);

    // Set steppers as OUTPUT and reset stepper states if all are done
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
    digitalWrite(in9Mega, HIGH);
    autosuareInitialised = false;
}
float lastTemperature = 0;
#ifdef READ_TEMPERATURES 
    void updateTemperatureSensor()
    {
        sensors.requestTemperatures();
        lastTemperature = sensors.getTempCByIndex(0);            
        #ifdef USE_DISPLAY 
            display.setFont(defaultFont);
            display.drawUTF8(0,3, "Temp:");
            display.setCursor(6,3);
            display.print(lastTemperature);
            display.drawString(10,3,"Grad");
        #endif
        lastReadTemperature = millis();
    }
#endif   

#ifdef I2C_TO_WIRELESS_ADAPTER
   void answerI2cCommunication() {
        dataFromController.onboardTemperature = 0;
        dataFromController.externalTemperature = lastTemperature;
        dataFromController.fanSpeed = 100;
        dataFromController.x1 = steppers[0].state;
        dataFromController.x2 = steppers[1].state;
        dataFromController.y1 = steppers[2].state;
        dataFromController.y2 = steppers[3].state;
        dataFromController.z1 = AUTOSQUARESTATE::off;
        dataFromController.z2 = AUTOSQUARESTATE::off;
        Wire.write((byte*)&dataFromController, sizeof(DATA_FROM_CONTROLLER));  
    }
    /**
     *  Try to communicate to Wireless Adapter via i2c
     **/ 
    void initialiseI2c() {
        Wire.begin(10);                // join i2c bus with address #8
        Wire.onRequest(answerI2cCommunication); 
    }
#endif

void setup()
{
    Serial.begin(9600);
    #ifdef READ_TEMPERATURES 
        // Inititalise temperature sensors
        sensors.begin();
    #endif
    // Endstops and steppers
    for (byte i = 0; i < sizeof(steppers) / sizeof(STEPPER); i++)
    {
        pinMode(steppers[i].endStopPin, INPUT_PULLUP);
        pinMode(steppers[i].stepPin, OUTPUT);
    }

    // Inputs
    // pinMode(tempSensor, INPUT_PULLUP);
    pinMode(autosquareButton, INPUT_PULLUP);

    pinMode(asXOn, INPUT_PULLUP);
    pinMode(asYOn, INPUT_PULLUP);

    // Check if autosuare for axis is on
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

    // Outputs
    pinMode(asStop, OUTPUT);
    digitalWrite(asStop, LOW);
    pinMode(in9Mega, OUTPUT);
    digitalWrite(in9Mega, HIGH);
    #ifdef USE_DISPLAY 
        // init display
        initDisplay();
    #endif
    #ifdef READ_TEMPERATURES 
        updateTemperatureSensor();
    #endif

    #ifdef I2C_TO_WIRELESS_ADAPTER
        initialiseI2c();
    #endif
}

void loop()
{
    // Check if button is pressed
    if (digitalRead(autosquareButton) == LOW)
    {
        // Check if start button was just pressed
        if(timeStartButtonWasPressed == 0) {
            timeStartButtonWasPressed = millis();
        }

        // Check if button was pressed long enough
        if(millis() - timeStartButtonWasPressed > AS_START_DELAY_MS) {
            // Check if we have to initinalize
            if (!autosuareInitialised)
            {
                initialize();
                delay(10);
            }            
            autosquare();
        }
    }
    else
    {
        // reset timeStartButtonWasPressed
        timeStartButtonWasPressed = 0;
        // terminate if initialized
        if (autosuareInitialised)
        {
            terminate();
        }
        #ifdef READ_TEMPERATURES 
            if(millis() - lastReadTemperature > TEMP_SENSOR_UPDATE_INTERVAL) 
            {
                updateTemperatureSensor();
            }
        #endif
    } 
}
