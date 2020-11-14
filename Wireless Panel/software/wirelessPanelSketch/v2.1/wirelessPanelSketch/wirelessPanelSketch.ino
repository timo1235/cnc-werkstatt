/**
 * Wireless Panel Firmware 2.1
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 * 
 * ***********************    General Notes    *****************************
 * - Uncomment means removing the 2 // in front of #define.
 * 
 * !! THE WIRELESS PANEL AND WIRELESS ADAPTER SKETCH SHOULD ALWAYS HAVE THE SAME FIRMWARE VERSION !!
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
// RF24 by TMRh20 -> https://tmrh20.github.io/RF24/
#include <RF24.h>
// The Rotary Encoder library should be included with this firmware
#include "RotaryEncoder.h"
// The CNC-Library should be included with this firmware
#include "cnc-library.h"

//===========================================================================
// *************************   Configuration    *****************************
//===========================================================================
// Debugging. If enabled, the sketch will print debug states to the serial monitor
// with 9600 Baudrate
// Default: commented 
// #define DEBUG                         // Uncomment to output debug messages

//===========================================================================
// *************************   Display    ***********************************
//===========================================================================
// Uncomment the next line only, if a display is connected by a wire with the shield.
// For example if the extension panel is used. If the wireless adapter is connected
// to the shield the next line has to be commented like this: "//#define USE_DISPLAY"
// Default: uncommented
#define USE_DISPLAY // comment with "//" if display is not needed

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
    #define DISPLAY_LAYOUT DISPLAYLAYOUT::Layout_128X64_1

    // We need to choose the right class for the display here.
    // Uncomment only one of them. If you have another display, 
    // take a look at the U8g2 documentation.
    // 128x32 Display
    // U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); 
    // 128x64 Display
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#endif

// Initial States - Which state should the outputs have, when connection is lost
// Normally you dont need to edit this. The defaults should be fine
DATA_TO_PANEL initialData = {
    0,   // unsigned ledX:1;
    0,   // unsigned ledY:1;
    0,   // unsigned ledZ:1;
    0,   // unsigned dtp1:1;
    0,   // unsigned dtp2:1;
    0,   // unsigned dtp3:1;
    0    // unsigned dtp4:1;
};

//===========================================================================
// ********** Only edit below, if you know what you are doing ***************
//===========================================================================
#define USE_RADIO 

// Pin configuration Inputs
const byte joystickX = A0;
const byte joystickY = A1;
const byte joystickZ = A2;
const byte feedrate = A3;
const byte rotationSpeed = A4;
const byte autosquareSpeed = A5;
const byte atc1 = A6; 
const byte atc2 = A7;
const byte autosquare = 45;
const byte in15 = 44;
const byte in16 = 43;
const byte ena = 39;
const byte auswahlX = 38;
const byte auswahlY = 37;
const byte auswahlZ = 36;
const byte ok = 17;
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
const byte speed3 = 31;
const byte speed4 = 14;
const byte speed5 = 15;
const byte speed6 = 16;
const byte encoderA = 9;
const byte encoderB = 10;
// Pin configuration Outputs
const byte ledX = 40;
const byte ledY = 41;
const byte ledZ = 42;
const byte dtp1 = 49;
const byte dtp2 = 48;
const byte dtp3 = 47;
const byte dtp4 = 46;
const byte ledEna = 30;
const byte ledAutosquare = 22;
const byte ledMotorStart = 7;
const byte ledProgrammStart = 8;
// Onboard LEDs
LEDControl statusLed = LEDControl(A9);
LEDControl powerLed = LEDControl(A8);

RotaryEncoder encoder(encoderA, encoderB);

// CNC radio configuration
CNCRadio radio = CNCRadio(3, 4, ROLE::panel);

// Messages
DATA_TO_CONTROL dataToControl;
DATA_TO_PANEL dataToPanel;
DATA_TO_PANEL lastDataToPanel;
DATA_FOR_DISPLAY dataForDisplay;

// Variables
#ifdef DEBUG
    uint32_t receivedPackages = 0;
    uint32_t timeLastDebugOutput = 0;
#endif
int64_t lastEncoderPosition = 0;
bool autosquareStarted = false;

void readInputs(){
  dataToControl.joystickX = analogRead(joystickX);
  dataToControl.joystickY = analogRead(joystickY);
  dataToControl.joystickZ = analogRead(joystickZ);
  dataToControl.feedrate = analogRead(feedrate);
  dataToControl.rotationSpeed = analogRead(rotationSpeed);
  dataToControl.autosquareSpeed = analogRead(autosquareSpeed);
  dataToControl.atc1 = analogRead(atc1);
  dataToControl.atc2 = analogRead(atc2);
  dataToControl.autosquare = !digitalRead(autosquare);
  dataToControl.in15 = !digitalRead(in15);
  dataToControl.in16 = !digitalRead(in16);
  dataToControl.ena = !digitalRead(ena);
  dataToControl.auswahlX = !digitalRead(auswahlX);
  dataToControl.auswahlY = !digitalRead(auswahlY);
  dataToControl.auswahlZ = !digitalRead(auswahlZ);
  dataToControl.ok = !digitalRead(ok);
  dataToControl.programmStart = !digitalRead(programmStart);
  dataToControl.programmStop = !digitalRead(programmStop);
  dataToControl.motorStart = !digitalRead(motorStart);
  dataToControl.motorStop = !digitalRead(motorStop);
  dataToControl.dtc1 = !digitalRead(dtc1);
  dataToControl.dtc2 = !digitalRead(dtc2);
  dataToControl.dtc3 = !digitalRead(dtc3);
  dataToControl.dtc4 = !digitalRead(dtc4);
  dataToControl.dtc5 = !digitalRead(dtc5);
  dataToControl.dtc6 = !digitalRead(dtc6);
  dataToControl.dtc7 = !digitalRead(dtc7);
  dataToControl.speed1 = !digitalRead(speed1);
  dataToControl.speed2 = !digitalRead(speed2);
  dataToControl.speed3 = !digitalRead(speed3);
  dataToControl.speed4 = !digitalRead(speed4);
  dataToControl.speed5 = !digitalRead(speed5);
  dataToControl.speed6 = !digitalRead(speed6);

  // Set LED states
  digitalWrite(ledEna ,dataToControl.ena);

  // Calculate encoder steps
  int16_t actualEncoderPosition = encoder.getPosition();
  if(actualEncoderPosition != lastEncoderPosition) {
    dataToControl.encoderMove = actualEncoderPosition - lastEncoderPosition;
    lastEncoderPosition = actualEncoderPosition;
  } else {
      dataToControl.encoderMove = 0;
  }
}

void writeOutputs(DATA_TO_PANEL *data) {
    if(lastDataToPanel.ledX != data->ledX) {
        digitalWrite(ledX, data->ledX);
    }
    if(lastDataToPanel.ledY != data->ledY) {
        digitalWrite(ledY, data->ledY);
    }
    if(lastDataToPanel.ledZ != data->ledZ) {
        digitalWrite(ledZ, data->ledZ);
    }
    if(lastDataToPanel.dtp1 != data->dtp1) {
        digitalWrite(dtp1, data->dtp1);
    }
    if(lastDataToPanel.dtp2 != data->dtp2) {
        digitalWrite(dtp2, data->dtp2);
    }
    if(lastDataToPanel.dtp3 != data->dtp3) {
        digitalWrite(dtp3, data->dtp3);
    }
    if(lastDataToPanel.dtp4 != data->dtp4) {
        digitalWrite(dtp4, data->dtp4);
    }

    lastDataToPanel = *data;
}

void setDataForDisplay(DATA_TO_PANEL *data) {
    dataForDisplay.externalTemperature = data->externalTemperature;
    dataForDisplay.onboardTemperature = data->onboardTemperature;
    dataForDisplay.fanSpeed = data->fanSpeed;
    dataForDisplay.stateX1 = data->stateX1;
    dataForDisplay.stateX2 = data->stateX2;
    dataForDisplay.stateY1 = data->stateY1;
    dataForDisplay.stateY2 = data->stateY2;
    dataForDisplay.stateZ1 = data->stateZ1;
    dataForDisplay.stateZ2 = data->stateZ2;
}

void readAndSendAck() {
    statusLed.setState(LEDControl::STATE::OFF);
    byte pipeNo;                                            // Declare variables for the pipe and the byte received
    while(radio.radio->available(&pipeNo)){                 // Read all available payloads
        statusLed.setState(LEDControl::STATE::ON);
        radio.radio->read(&dataToPanel, sizeof(DATA_TO_PANEL));  
        writeOutputs(&dataToPanel); 
        setDataForDisplay(&dataToPanel);                
        // Since this is a call-response. Respond directly with an ack payload.
        // Ack payloads are much more efficient than switching to transmit mode to respond to a call
        readInputs();
        radio.radio->writeAckPayload(pipeNo,&dataToControl, sizeof(DATA_TO_CONTROL));  // This can be commented out to send empty payloads.
        #ifdef DEBUG
            receivedPackages++;
        #endif
   }
}

#include "cnclibrary_common_functions.h"

void setup() {
    radio.setup();
    powerLed.setup();
    statusLed.setup();

    #ifdef USE_DISPLAY
        displaySetup();
    #endif

    #ifdef DEBUG
        Serial.begin(9600);
    #endif   

    checkRadioConnection();
    
    // Inputs
    pinMode(autosquare, INPUT_PULLUP);
    pinMode(in15, INPUT_PULLUP);
    pinMode(in16, INPUT_PULLUP);
    pinMode(ena, INPUT_PULLUP);
    pinMode(auswahlX, INPUT_PULLUP);
    pinMode(auswahlY, INPUT_PULLUP);
    pinMode(auswahlZ, INPUT_PULLUP);
    pinMode(ok, INPUT_PULLUP);
    pinMode(programmStart, INPUT_PULLUP);
    pinMode(programmStop, INPUT_PULLUP);
    pinMode(motorStart, INPUT_PULLUP);
    pinMode(motorStop, INPUT_PULLUP);
    pinMode(dtc1, INPUT_PULLUP);
    pinMode(dtc2, INPUT_PULLUP);
    pinMode(dtc3, INPUT_PULLUP);
    pinMode(dtc4, INPUT_PULLUP);
    pinMode(dtc5, INPUT_PULLUP);
    pinMode(dtc6, INPUT_PULLUP);
    pinMode(dtc7, INPUT_PULLUP);
    pinMode(speed1, INPUT_PULLUP);
    pinMode(speed2, INPUT_PULLUP);
    pinMode(speed3, INPUT_PULLUP);
    pinMode(speed4, INPUT_PULLUP);
    pinMode(speed5, INPUT_PULLUP);
    pinMode(speed6, INPUT_PULLUP);

    //Outputs
    pinMode(ledX, OUTPUT);
    pinMode(ledY, OUTPUT);
    pinMode(ledZ, OUTPUT);
    pinMode(dtp1, OUTPUT);
    pinMode(dtp2, OUTPUT);
    pinMode(dtp3, OUTPUT);
    pinMode(dtp4, OUTPUT);
    // Self controlled outputs
    pinMode(ledEna, OUTPUT);
    pinMode(ledAutosquare, OUTPUT);
    pinMode(ledMotorStart, OUTPUT);
    pinMode(ledProgrammStart, OUTPUT);
    digitalWrite(ledEna ,LOW);
    digitalWrite(ledAutosquare ,HIGH);
    digitalWrite(ledMotorStart ,HIGH);
    digitalWrite(ledProgrammStart ,HIGH);

    // Set power led on
    powerLed.setState(LEDControl::STATE::ON);

    writeOutputs(&initialData);
}

void loop() {    
    powerLed.loop();
    statusLed.loop();
    encoder.tick();
    checkRadioConnection();

    readAndSendAck(); 

    #ifdef USE_DISPLAY
        // If we have the small display show autosquaring state while autosquaring
        if(dataToControl.autosquare == 1 && DISPLAY_LAYOUT == DISPLAYLAYOUT::Layout_128X32_Fan_and_Temp) {
            autosquareStarted = true;
            displayUpdate(DISPLAYLAYOUT::Layout_128X32_Autosquare);
        } else {
            if(autosquareStarted) {
                displayUpdate(DISPLAY_LAYOUT, true);
            } else {
                displayUpdate(DISPLAY_LAYOUT);
            }        
            autosquareStarted = false;
        }    
    #endif

    #ifdef DEBUG
        // Show package statistics every 10 seconds
        if (millis() - timeLastDebugOutput > 10000) {            
            Serial.print("Received packages: ");
            Serial.print(receivedPackages);
            Serial.print(", in ");
            Serial.print((millis() - timeLastDebugOutput) / 1000);
            Serial.print(" seconds, ");
            Serial.print((receivedPackages) / ((millis() - timeLastDebugOutput) / 1000));
            Serial.println(" packages/second");
            timeLastDebugOutput = millis();
            receivedPackages = 0;
        }
    #endif
}
