/*
    Tillboard Extension Firmware 2.0
    find out more: blog.altholtmann.com
    
    by Timo Altholtmann
*/

#include <Arduino.h>
#include "cnc-library.h"


// Programm configuration
uint16_t stepDelay = 0; // delay between steps in microseconds if no poti is connected - set to 0 to use a poti for setting speed
uint16_t startDelay = 1000; // delay between pressing the autosquare button and starting autosquare - in miliseconds, should be at least 500 
byte speedPotiSamples = 10; // Samples of every speed poti reading
uint16_t speedPotiMin = 10;  // min delay between steps in microseconds
uint16_t speedPotiMax = 5000; // max delay between steps in microseconds
uint16_t tempSensorUpdateInterval = 5000; // interval between temperatur sensor reads in milliseconds
byte endstopStateTriggered = LOW; // Use HIGH for normally open endstops and LOW for normally closed endstops

#define READ_TEMPERATURES // comment with "//" if temperature sensor is not needed

/**
 * USE_DISPLAY
 * Uncomment the next line only, if a display is connected by a wire with the shield. For example if the extension panel is used.
 * If the wireless solution is connected to shield the next line has to be commented like this: "//#define USE_DISPLAY"
 */
//#define USE_DISPLAY // comment with "//" if temperature sensor is not needed

/**
 * WIRELESS ADAPTER CONNECTED
 * Comment with // if wireless adapter is not connected!
 * IMPORTANT - Has to be commented if there is a wired connection on the sub-d port. For example if the wired extension panel is used
 */
//#define WIRELESS_ADAPTER_CONNECTED

// ------------------------------------------------------------------------------
// Only edit below if you know what you are doing
// ------------------------------------------------------------------------------
#ifdef WIRELESS_ADAPTER_CONNECTED
    #include <Wire.h>
#endif

#ifdef USE_DISPLAY 
    #include <U8x8lib.h>
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
#endif


// Programm variales
uint32_t actualStepDelay = 0;
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
                display.print(lastReadTemperature);
                display.drawString(10,3,"Grad");
            #endif
            lastReadTemperature = millis();
        
    }
#endif   

#ifdef WIRELESS_ADAPTER_CONNECTED
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

    #ifdef WIRELESS_ADAPTER_CONNECTED
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
        if(millis() - timeStartButtonWasPressed > startDelay) {
            // Check if we have to initinalize
            if (!autosuareInitialised)
            {
                initialize();
                delay(10);
            }            
            autosquare();
        } else {
            // Use waiting time to determine if 
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
            if(millis() - lastReadTemperature > tempSensorUpdateInterval) 
            {
                updateTemperatureSensor();
            }
        #endif
    } 
}
