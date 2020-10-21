#ifndef cnclibrary_ledstate_h
#define cnclibrary_ledstate_h

// includes
#include <Arduino.h>

class LEDControl
{
private:
    bool _actualLedState;
    byte _ledPin;
    void _toggle();
    void _setLed(bool state);
    uint32_t _lastSwitchedOn_MS;
    uint32_t _lastSwitchedOff_MS;
    void _handleActiveState();
    uint16_t _activeStateTimeout = 50;
    uint16_t _minStateChangeDelay = 30;
public:
    enum STATE {
        NRF24Failure,
        ON,
        OFF,
        ACTIVE
    } state;
    LEDControl(byte ledPin);
    void setup();
    void loop();
    void setState(STATE state);
    ~LEDControl();
};

/**
 * Implementations
 */
void LEDControl::setup() {
    this->_actualLedState = false;
    this->state = STATE::OFF;
    pinMode(this->_ledPin, OUTPUT);
    this->_setLed(this->_actualLedState);
}

void LEDControl::loop() {
    if(this->state == STATE::ON && this->_actualLedState == false && millis() - this->_lastSwitchedOff_MS > _minStateChangeDelay) {
        this->_setLed(true);
    } else if (this->state == STATE::OFF && this->_actualLedState == true  && millis() - this->_lastSwitchedOn_MS > _minStateChangeDelay) {
        this->_setLed(false);
    } else if(this->state == STATE::NRF24Failure) {
        while (true)
        {
            this->_toggle();
            delay(1000);
            this->_toggle();
            delay(1000);
        }
    }else if(this->state == STATE::ACTIVE) {
        this->_handleActiveState();
    }
}

void LEDControl::_handleActiveState() {
    if(this->_actualLedState && millis() - this->_lastSwitchedOn_MS > _activeStateTimeout) {
        this->_setLed(false);
    } else if(!this->_actualLedState && millis() - this->_lastSwitchedOff_MS > _activeStateTimeout) {
        this->_setLed(true);
    }
}

void LEDControl::_toggle() {
    this->_setLed(!this->_actualLedState);
}

void LEDControl::_setLed(bool state) {
    if(state) {
        this->_lastSwitchedOn_MS = millis();
        this->_lastSwitchedOff_MS = 0;
    } else {
        this->_lastSwitchedOn_MS = 0;
        this->_lastSwitchedOff_MS = millis();
    }
    this->_actualLedState = state;
    digitalWrite(this->_ledPin, this->_actualLedState);
}

/** 
 * Set the state of the led(on/off).
 * @param state true: led is on / false: led is off
 */
void LEDControl::setState(STATE state) {
    this->state = state;
}

/** 
 * Constructor for the LEDState class.
 * @param ledPin The pin the led is attached to
 */
LEDControl::LEDControl(byte ledPin)
{
    this->_ledPin = ledPin;
}

/** 
 * Destructor of the LEDState class.
 */
LEDControl::~LEDControl()
{
}

#endif