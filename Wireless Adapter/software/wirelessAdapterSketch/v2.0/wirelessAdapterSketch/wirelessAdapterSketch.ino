/**
 * Wireless Adapter Firmware 2.0
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 **/

#include <Arduino.h>
#include <RF24.h>
#include "mcp4728.h"
#include <Wire.h>
#include "cnc-library.h"

// Configuration
const uint32_t wirelessTimeout = 500000; // in microseconds(1000 micros = 1 ms)If this timeout is reached, the outputs will be set to their initial state again
const uint32_t wirelessDelay = 10;      // The delay between wireless state updates in milliseconds
const bool reverseEnaState = false;      // Set to true if your drivers need a different Enable state to be enabled
// Set to true if you want to use IN15/IN16 on the wireless panel. If set to true, the terminals for IN15 and IN16 on the 
// cnc shield itself cant be used anymore
const bool useIn15Wireless = false;     
const bool useIn16Wireless = false;
// Wheater to reset also the feed rate and speed poti when timeout occurs
const bool shouldResetSpeedAndFeedRatePotiWhenTimedOut = false;
// #define DEBUG ;                         // Uncomment to output debug messages

#define USE_RADIO;

// Initial States - Which state should the outputs have, when connection is lost
DATA_TO_CONTROL initialData = {
    512, // uint16_t joystickX;
    512, // uint16_t joystickY;
    512, // uint16_t joystickZ;
    0,   // uint16_t feedrate;
    0,   // uint16_t rotationSpeed;
    0,   // uint16_t atc1;
    0,   // uint16_t atc2;
    0,   // uint16_t autosquareSpeed;
    0,   // int16_t encoderMove;
    0,   // unsigned autosquare : 1;
    0,   // unsigned in15 : 1;
    0,   // unsigned in16 : 1;
    0,   // unsigned ena : 1;
    0,   // unsigned auswahlX : 1;
    0,   // unsigned auswahlY : 1;
    0,   // unsigned auswahlZ : 1;
    0,   // unsigned ok : 1;
    0,   // unsigned programmStart : 1;
    0,   // unsigned programmStop : 1;
    0,   // unsigned motorStart : 1;
    0,   // unsigned motorStop : 1;
    0,   // unsigned dtc1 : 1;
    0,   // unsigned dtc2 : 1;
    0,   // unsigned dtc3 : 1;
    0,   // unsigned dtc4 : 1;
    0,   // unsigned dtc5 : 1;
    0,   // unsigned dtc6 : 1;
    0,   // unsigned dtc7 : 1;
    0,   // unsigned speed1 : 1;
    0,   // unsigned speed2 : 1;
    0,   // unsigned speed3 : 1;
    0,   // unsigned speed4 : 1;
    0,   // unsigned speed5 : 1;
    0    // unsigned speed6 : 1;
};

// ------------------------------------------------------------------------------
// Only edit below if you know what you are doing
// ------------------------------------------------------------------------------

// Pin configuration Outputs
const byte autosquare = 45;
const byte in15 = 44;
const byte in16 = 43;
const byte ena = 39;
const byte auswahlX = 38;
const byte auswahlY = 37;
const byte auswahlZ = 36;
const byte ok = 18;
const byte programmStart = 34;
const byte programmStop = 35;
const byte motorStart = 32;
const byte motorStop = 33;
const byte dtc1 = 23;
const byte dtc2 = 24;
const byte dtc3 = 25;
const byte dtc4 = 26;
const byte dtc5 = 27;
const byte dtc6 = 28;
const byte dtc7 = 29;
const byte speed1 = 11;
const byte speed2 = 12;
const byte speed3 = 14;
const byte speed4 = 15;
const byte speed5 = 16;
const byte speed6 = 17;
const byte encoderA = 9;
const byte encoderB = 10;
// Power of MCP Chip
const byte mcp1Pin = 19;
const byte mcp2Pin = 22;
const byte mcp1ldac = 5;
const byte mcp2ldac = 6;
// Pin configuration Inputs
const byte ledX = 40;
const byte ledY = 41;
const byte ledZ = 42;
const byte dtp1 = 46;
const byte dtp2 = 47;
const byte dtp3 = 48;
const byte dtp4 = 49;
// Onboard LEDs
LEDControl statusLed = LEDControl(A15);
LEDControl powerLed = LEDControl(A14);
// NRF Radio
CNCRadio radio = CNCRadio(3, 4, ROLE::adapter);

mcp4728 mcp1 = mcp4728(0);
mcp4728 mcp2 = mcp4728(1);

// Messages
DATA_TO_CONTROL dataToControl;
DATA_TO_CONTROL lastDataToControl; // Keep track of data and write only if data changed
DATA_TO_PANEL dataToPanel;
DATA_FROM_CONTROLLER dataFromController;

// Variables
#ifdef DEBUG
    uint16_t successfulPackages = 0;
    uint16_t failedPackages = 0;
    uint32_t timeLastDebugOutput = 0;
    uint32_t resetedStateCounter = 0;
#endif
unsigned long timeLastSuccessfullyReceived = 0;
unsigned long timeLastWirelessUpdate = 0;
int32_t nextEncoderMove = 0;
bool lastCommunicationSuccessful = false;

void writeOutputs(DATA_TO_CONTROL *data, bool forceWrite = false)
{
    // Many outputs are reversed, since the tillboard extension or open cnc shield pins are switched with GND

    if(lastDataToControl.joystickX != data->joystickX 
        || lastDataToControl.joystickY != data->joystickY
        || lastDataToControl.joystickZ != data->joystickZ
        || lastDataToControl.feedrate != data->feedrate
        || forceWrite
    ){ 
        mcp1.analogWrite(
            map(data->joystickX, 0, 1023, 0, 4095),
            map(data->joystickY, 0, 1023, 0, 4095),
            map(data->joystickZ, 0, 1023, 0, 4095),
            map(data->feedrate, 0, 1023, 0, 4095)
        );
    }
    if(lastDataToControl.rotationSpeed != data->rotationSpeed 
        || lastDataToControl.autosquareSpeed != data->autosquareSpeed
        || lastDataToControl.atc2 != data->atc2
        || lastDataToControl.atc1 != data->atc1
        || forceWrite
    ){ 
        mcp2.analogWrite(
            map(data->rotationSpeed, 0, 1023, 0, 4095),
            map(data->autosquareSpeed, 0, 1023, 0, 4095),
            map(data->atc2, 0, 1023, 0, 4095),
            map(data->atc1, 0, 1023, 0, 4095)
        );
    }
    if(lastDataToControl.autosquare != data->autosquare || forceWrite) {
        digitalWrite(autosquare, !data->autosquare);
    }
    if(useIn15Wireless) {
        if(lastDataToControl.in15 != data->in15 || forceWrite) {
            digitalWrite(in15, !data->in15);
        }
    }
    if(useIn16Wireless) {
        if(lastDataToControl.in16 != data->in16 || forceWrite) {
            digitalWrite(in16, !data->in16);
        }
    }        
    if(lastDataToControl.ena != data->ena || forceWrite) {
        digitalWrite(ena, reverseEnaState ? !data->ena : data->ena);
    }
    if(lastDataToControl.auswahlX != data->auswahlX || forceWrite) {
        digitalWrite(auswahlX, !data->auswahlX);
    }
    if(lastDataToControl.auswahlY != data->auswahlY || forceWrite) {
        digitalWrite(auswahlY, !data->auswahlY);
    }
    if(lastDataToControl.auswahlZ != data->auswahlZ || forceWrite) {
        digitalWrite(auswahlZ, !data->auswahlZ);
    }
    if(lastDataToControl.ok != data->ok || forceWrite) {
        digitalWrite(ok, !data->ok);
    }
    if(lastDataToControl.programmStart != data->programmStart || forceWrite) {
        digitalWrite(programmStart, !data->programmStart);
    }
    if(lastDataToControl.programmStop != data->programmStop || forceWrite) {
        digitalWrite(programmStop, !data->programmStop);
    }
    if(lastDataToControl.motorStart != data->motorStart || forceWrite) {
        digitalWrite(motorStart, !data->motorStart);
    }
    if(lastDataToControl.motorStop != data->motorStop || forceWrite) {
        digitalWrite(motorStop, !data->motorStop);
    }
    if(lastDataToControl.dtc1 != data->dtc1 || forceWrite) {
        digitalWrite(dtc1, data->dtc1);
    }
    if(lastDataToControl.dtc2 != data->dtc2 || forceWrite) {
        digitalWrite(dtc2, data->dtc2);
    }
    if(lastDataToControl.dtc3 != data->dtc3 || forceWrite) {
        digitalWrite(dtc3, data->dtc3);
    }
    if(lastDataToControl.dtc4 != data->dtc4 || forceWrite) {
        digitalWrite(dtc4, data->dtc4);
    }
    if(lastDataToControl.dtc5 != data->dtc5 || forceWrite) {
        digitalWrite(dtc5, data->dtc5);
    }
    if(lastDataToControl.dtc6 != data->dtc6 || forceWrite) {
        digitalWrite(dtc6, data->dtc6);
    }
    if(lastDataToControl.dtc7 != data->dtc7 || forceWrite) {
        digitalWrite(dtc7, data->dtc7);
    }
    if(lastDataToControl.speed1 != data->speed1 || forceWrite) {
        digitalWrite(speed1, !data->speed1);
    }
    if(lastDataToControl.speed2 != data->speed2 || forceWrite) {
        digitalWrite(speed2, !data->speed2);
    }
    if(lastDataToControl.speed3 != data->speed3 || forceWrite) {
        digitalWrite(speed3, !data->speed3); 
    }
    if(lastDataToControl.speed4 != data->speed4 || forceWrite) {
        digitalWrite(speed4, !data->speed4);
    }
    if(lastDataToControl.speed5 != data->speed5 || forceWrite) {
        digitalWrite(speed5, !data->speed5);
    }
    if(lastDataToControl.speed6 != data->speed6 || forceWrite) {
        digitalWrite(speed6, !data->speed6);
    }
    if(data->encoderMove != 0) {
        nextEncoderMove+= data->encoderMove;
    }    

    lastDataToControl = *data;
}
void readInputs()
{
    dataToPanel.ledX = digitalRead(ledX);
    dataToPanel.ledY = digitalRead(ledY);
    dataToPanel.ledZ = digitalRead(ledZ);
    dataToPanel.dtp1 = !digitalRead(dtp1);
    dataToPanel.dtp2 = !digitalRead(dtp2);
    dataToPanel.dtp3 = !digitalRead(dtp3);
    dataToPanel.dtp4 = !digitalRead(dtp4);
}
void setAdditionalDataForPanel() {
    dataToPanel.externalTemperature = dataFromController.externalTemperature;
    dataToPanel.onboardTemperature = dataFromController.onboardTemperature;
    dataToPanel.fanSpeed = map(dataFromController.fanSpeed, 100, 255, 1, 7);
    dataToPanel.stateX1 = dataFromController.x1;
    dataToPanel.stateX2 = dataFromController.x2;
    dataToPanel.stateY1 = dataFromController.y1;
    dataToPanel.stateY2 = dataFromController.y2;
    dataToPanel.stateZ1 = dataFromController.z1;
    dataToPanel.stateZ2 = dataFromController.z2;
}
void sendAndGetRequest() {
    readInputs();
    setAdditionalDataForPanel();
    if (radio.radio->write(&dataToPanel, sizeof(DATA_TO_PANEL)))
    {
        if (radio.radio->available())
        {
            // If an ack with payload was received
            radio.radio->read(&dataToControl, sizeof(DATA_TO_CONTROL));
            writeOutputs(&dataToControl);
            timeLastSuccessfullyReceived = micros();
            lastCommunicationSuccessful = true;
            statusLed.setState(LEDControl::STATE::ON);
            #ifdef DEBUG
                successfulPackages++;
            #endif
        }
    }
    else
    {
        // Transmitting failed - If timeout is reached, everything is set back to the initial state
        // Only reset if last communication was successful and we now enter failed state
        if (micros() - timeLastSuccessfullyReceived > wirelessTimeout && lastCommunicationSuccessful == true)
        {
            // dont reset feed rate poti and speed poti
            if(!shouldResetSpeedAndFeedRatePotiWhenTimedOut) {
                initialData.feedrate = lastDataToControl.feedrate;
                initialData.rotationSpeed = lastDataToControl.rotationSpeed;
            }
            writeOutputs(&initialData);
            if(!shouldResetSpeedAndFeedRatePotiWhenTimedOut) {
                initialData.feedrate = 0;
                initialData.rotationSpeed = 0;
            }
            #ifdef DEBUG
                resetedStateCounter++;
            #endif
            lastCommunicationSuccessful = false;
        }   
        #ifdef DEBUG
            failedPackages++;  
        #endif   
    }
}

void moveEncoder() {
    if(nextEncoderMove > 0) {
        nextEncoderMove--;
        delayMicroseconds(200); 
        digitalWrite(encoderB, HIGH);
        delayMicroseconds(200); 
        digitalWrite(encoderA, HIGH);
        delayMicroseconds(200); 
        digitalWrite(encoderB, LOW);
        delayMicroseconds(200); 
        digitalWrite(encoderA, LOW);        
    } else {
        nextEncoderMove++;
        delayMicroseconds(200); 
        digitalWrite(encoderA, HIGH);
        delayMicroseconds(200); 
        digitalWrite(encoderB, HIGH);
        delayMicroseconds(200); 
        digitalWrite(encoderA, LOW);
        delayMicroseconds(200); 
        digitalWrite(encoderB, LOW);        
    }   
}

void initMcpChips() {    
    Serial.println("Start init MCP");
    // MCP4728
    pinMode(mcp1Pin, OUTPUT);
    pinMode(mcp2Pin, OUTPUT);
    pinMode(mcp1ldac, OUTPUT);
    pinMode(mcp2ldac, OUTPUT);
    // Power on mcp chips
    digitalWrite(mcp1Pin, HIGH);
    digitalWrite(mcp2Pin, HIGH);

    // set ldac to gnd
    digitalWrite(mcp1ldac, LOW);
    digitalWrite(mcp2ldac, LOW);
    
    mcp1.begin();  
    mcp1.vdd(5000);
    mcp1.setVref(0,0,0,0);

    mcp2.begin();  
    mcp2.vdd(5000);  
    mcp2.setVref(0,0,0,0); 
    Serial.println("Finish init MCP");
}

#include "cnclibrary_common_functions.h"

uint32_t lastControllerCommunication = 0;
void communicateWithController();

void setup()
{
    radio.setup();
    powerLed.setup();
    statusLed.setup();


    #ifdef DEBUG
        Serial.begin(9600);
    #endif

    checkRadioConnection();

    //Inputs
    pinMode(ledX, INPUT);
    pinMode(ledY, INPUT);
    pinMode(ledZ, INPUT);
    pinMode(dtp1, INPUT_PULLUP);
    pinMode(dtp2, INPUT_PULLUP);
    pinMode(dtp3, INPUT_PULLUP);
    pinMode(dtp4, INPUT_PULLUP);

    // Outputs
    pinMode(autosquare, OUTPUT);
    useIn15Wireless ? pinMode(in15, OUTPUT) : pinMode(in15, INPUT);
    useIn16Wireless ? pinMode(in16, OUTPUT) : pinMode(in16, INPUT);
   
    pinMode(ena, OUTPUT);
    pinMode(auswahlX, OUTPUT);
    pinMode(auswahlY, OUTPUT);
    pinMode(auswahlZ, OUTPUT);
    pinMode(ok, OUTPUT);
    pinMode(programmStart, OUTPUT);
    pinMode(programmStop, OUTPUT);
    pinMode(motorStart, OUTPUT);
    pinMode(motorStop, OUTPUT);
    pinMode(dtc1, OUTPUT);
    pinMode(dtc2, OUTPUT);
    pinMode(dtc3, OUTPUT);
    pinMode(dtc4, OUTPUT);
    pinMode(dtc5, OUTPUT);
    pinMode(dtc6, OUTPUT);
    pinMode(dtc7, OUTPUT);
    pinMode(speed1, OUTPUT);
    pinMode(speed2, OUTPUT);
    pinMode(speed3, OUTPUT);
    pinMode(speed4, OUTPUT);
    pinMode(speed5, OUTPUT);
    pinMode(speed6, OUTPUT);
    pinMode(encoderA, OUTPUT);
    pinMode(encoderB, OUTPUT);

    digitalWrite(encoderA, LOW);
    digitalWrite(encoderB, LOW);

    // Initialise DAC Chips
    initMcpChips();
    writeOutputs(&initialData, true);

    // Set power led on
    powerLed.setState(LEDControl::STATE::ON);

    Wire.begin();
    Wire.setWireTimeout(3000, true);
    Wire.clearWireTimeoutFlag();
}

void loop() {
    powerLed.loop();
    statusLed.loop();
    checkRadioConnection();
    communicateWithController();

    #ifdef DEBUG
        // Show package statistics every 10 seconds
        if (millis() - timeLastDebugOutput > 10000)
        {
            Serial.print("Successful packages: ");
            Serial.print(successfulPackages);
            Serial.print(", failed packages: ");
            Serial.print(failedPackages);
            Serial.print(" Resetted State: ");
            Serial.print(resetedStateCounter);
            Serial.print(" times, in ");
            Serial.print((millis() - timeLastDebugOutput) / 1000);
            Serial.print(" seconds, ");
            Serial.print((successfulPackages + failedPackages) / ((millis() - timeLastDebugOutput) / 1000));
            Serial.println(" packages/second");
            timeLastDebugOutput = millis();
            successfulPackages = 0;
            failedPackages = 0;
            resetedStateCounter = 0;
        }
    #endif

    if(nextEncoderMove != 0) {
        moveEncoder();
    }  

    if(millis() - timeLastWirelessUpdate > wirelessDelay || lastCommunicationSuccessful == false) {
        timeLastWirelessUpdate = millis();
        sendAndGetRequest();
    } else {
        statusLed.setState(LEDControl::STATE::OFF);
    }
}

void communicateWithController() {
    if(millis() - lastControllerCommunication > 1000) {
        // Serial.println("Communicate with controller");
        Wire.requestFrom(10, sizeof(DATA_FROM_CONTROLLER));

        Wire.readBytes((byte*)&dataFromController, sizeof(DATA_FROM_CONTROLLER));

        lastControllerCommunication = millis();
    }
    
}