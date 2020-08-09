/**
 * Wireless Panel Firmware 1.0
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 **/

#include <Arduino.h>
#include <RF24.h>
#include "RotaryEncoder.h"
#include <cnc-library.h>

// Configuration
#define DEBUG ; // Uncomment to output debug messages

#define USE_DISPLAY // comment with "//" if display is not needed

#ifdef USE_DISPLAY 
    #include <U8x8lib.h>
#endif


// Initial states
DATA_TO_PANEL initialData = {
    0,   // unsigned ledX:1;
    0,   // unsigned ledY:1;
    0,   // unsigned ledZ:1;
    0,   // unsigned dtp1:1;
    0,   // unsigned dtp2:1;
    0,   // unsigned dtp3:1;
    0    // unsigned dtp4:1;
};

// ------------------------------------------------------------------------------
// Only edit below if you know what you are doing
// ------------------------------------------------------------------------------
#ifdef USE_DISPLAY 
    // Display byte SDA = A4, SCL = A5
    // U8X8_SSD1306_128X32_UNIVISION_SW_I2C display(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
    U8X8_SSD1306_128X64_NONAME_SW_I2C display(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
    // Hardware i2c does not work for me with long wires
    // U8X8_SSD1306_128X32_UNIVISION_HW_I2C display(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);
    const uint8_t *defaultFont = u8x8_font_pxplustandynewtv_f;
#endif

#ifdef DEBUG
    #include <printf.h>
#endif

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
const byte speed3 = 13;
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
LEDState statusLed = LEDState(A8);
LEDState powerLed = LEDState(A9);

RotaryEncoder encoder(encoderA, encoderB);

// Radio configuration
RF24 radio(3, 4);   // nRF24L01 (CE, CSN)
uint8_t addresses[][6] = {"No1","No2"};

// Messages
DATA_TO_CONTROL dataToControl;
DATA_TO_PANEL dataToPanel;
DATA_TO_PANEL lastDataToPanel;

// Variables
#ifdef DEBUG
    uint32_t receivedPackages = 0;
    uint32_t timeLastDebugOutput = 0;
#endif
uint32_t timeLastNRF24Check = 0;
int64_t lastEncoderPosition = 0;

#ifdef USE_DISPLAY 
    void initDisplay() {
        display.begin();
        display.setFont(defaultFont);
        display.drawString(0, 0, "- Just Dummy yet -");
    }
#endif

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

void readAndSendAck() {
    byte pipeNo;                          // Declare variables for the pipe and the byte received
    while(radio.available(&pipeNo)){              // Read all available payloads
      radio.read(&dataToPanel, sizeof(DATA_TO_PANEL));  
      writeOutputs(&dataToPanel);                 
      // Since this is a call-response. Respond directly with an ack payload.
      // Ack payloads are much more efficient than switching to transmit mode to respond to a call
      readInputs();
      radio.writeAckPayload(pipeNo,&dataToControl, sizeof(DATA_TO_CONTROL));  // This can be commented out to send empty payloads.
      receivedPackages++;
   }
}

void setup() {
    #ifdef DEBUG
        Serial.begin(9600);
        printf_begin();
    #endif

    radio.begin();
    radio.setDataRate(RF24_2MBPS);
    radio.setPayloadSize(sizeof(DATA_TO_CONTROL));
    // radio.setRetries(0,15);
    radio.enableAckPayload();                     // Allow optional ack payloads
    radio.enableDynamicPayloads();                // Ack payloads are dynamic payloads
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    radio.startListening();

    checkNRF24Chip(&radio, &statusLed);

    // Inputs
    pinMode(autosquare, INPUT_PULLUP);
    pinMode(in15, INPUT_PULLUP);
    pinMode(in16, INPUT_PULLUP);
    // ToDo: Will be corrected in next pcb
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

    #ifdef USE_DISPLAY 
        // init display
        initDisplay();
    #endif

    // Set power led on
    powerLed.setState(true);

    writeOutputs(&initialData);
}

void loop() {    
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
    // Check NRF24 State every 10 seconds
    if (millis() - timeLastNRF24Check > 10000)
    {
        checkNRF24Chip(&radio, &statusLed);
        timeLastNRF24Check = millis();
    }
    encoder.tick();
    readAndSendAck(); 
}