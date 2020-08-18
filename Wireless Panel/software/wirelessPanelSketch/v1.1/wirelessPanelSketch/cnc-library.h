/**
 * Library for CNC related controls
 * Version 1.0
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 **/


#ifndef cnclibrary_h
#define cnclibrary_h

#include <Arduino.h>

/**
 * LED state class
 **/
enum BLINKSTATE
{
    NRF24Failure
};

class LEDState
{
private:
    byte actualState;
    byte ledPin;
public:
    LEDState(byte ledPin);
    void setEndlessBlinkState(BLINKSTATE state);
    void setState(bool state);
    ~LEDState();

};
void LEDState::setState(bool state) {
    digitalWrite(this->ledPin, state);
}

void LEDState::setEndlessBlinkState(BLINKSTATE state)
{
    if(state == NRF24Failure) {
        while (true)
        {
            this->setState(true);
            delay(1000);
            this->setState(false);
            delay(1000);
        }
    }
}

LEDState::LEDState(byte ledPin)
{
    this->ledPin = ledPin;
    pinMode(this->ledPin, OUTPUT);
    digitalWrite(this->ledPin, LOW);
}

LEDState::~LEDState()
{
}

/**
 * Common structs needed for wireless commnunication
 **/
// Max size of this struct is 32 bytes - NRF24L01 buffer limit
// uint16_t : 2Bytes
typedef struct {
    unsigned ledX : 1;
    unsigned ledY : 1;
    unsigned ledZ : 1;
    unsigned dtp1 : 1;
    unsigned dtp2 : 1;
    unsigned dtp3 : 1;
    unsigned dtp4 : 1;
} DATA_TO_PANEL;

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
// uint16_t : 2Bytes
// unsigned : 1 Bit each
typedef struct {
  uint16_t joystickX;
  uint16_t joystickY;
  uint16_t joystickZ;
  uint16_t feedrate;
  uint16_t rotationSpeed;
  uint16_t atc1;
  uint16_t atc2;
  uint16_t autosquareSpeed;
  int16_t encoderMove;
  unsigned autosquare:1;
  unsigned in15:1;
  unsigned in16:1;
  unsigned ena:1;
  unsigned auswahlX:1;
  unsigned auswahlY:1;
  unsigned auswahlZ:1;
  unsigned ok:1;
  unsigned programmStart:1;
  unsigned programmStop:1;
  unsigned motorStart:1;
  unsigned motorStop:1;
  unsigned dtc1:1;
  unsigned dtc2:1;
  unsigned dtc3:1;
  unsigned dtc4:1;
  unsigned dtc5:1;
  unsigned dtc6:1;
  unsigned dtc7:1;
  unsigned speed1:1;
  unsigned speed2:1;
  unsigned speed3:1;
  unsigned speed4:1;
  unsigned speed5:1;
  unsigned speed6:1;
} DATA_TO_CONTROL;

/**
 * shared functions
 **/
void checkNRF24Chip(RF24 *radio, LEDState *ledState, HardwareSerial *serial = NULL) {
    bool connected = radio->isChipConnected();

    if(!connected) {
        if(serial != NULL) {
            serial->println("NRF24 chip is not connected or broken");
            serial->println("Restart Arduino");
        }
        ledState->setEndlessBlinkState(NRF24Failure);          
    }

    if(serial != NULL) {
        serial->println("NRF24 chip connected");
    }
    ledState->setState(true);
}

#endif