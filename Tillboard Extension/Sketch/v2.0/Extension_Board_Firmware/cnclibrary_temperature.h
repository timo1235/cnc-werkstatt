#ifndef cnclibrary_temperature_h
#define cnclibrary_temperature_h

// includes
#include <Arduino.h>
#include <OneWire.h> 
#include "DallasTemperature.h"

/**
 * Common structs needed
 **/


class CNCTemperature {
private:
    byte _sensorPin;
    byte _numberOfSensors = 1;
    uint32_t _lastReadTemperature_MS = 0;
    uint32_t _interval_MS = 5000;
    OneWire _oneWire;       
    DallasTemperature _sensors;
    float _lastTemperatures[10];
public:
    CNCTemperature(byte sensorPin, byte numberOfSensors, uint32_t interval_MS);
    ~CNCTemperature();
    void setup();
    void loop();
    float getTemperature(byte sensorNumber);
};

void CNCTemperature::setup() {
    this->_sensors.begin();    
}

void CNCTemperature::loop() {
    if (millis() - this->_lastReadTemperature_MS > this->_interval_MS) {
        this->_sensors.requestTemperatures();
        for (int i = 0; i < this->_numberOfSensors; i++)
        {
            this->_lastTemperatures[i] = this->_sensors.getTempCByIndex(i);
        }
        
        this->_lastReadTemperature_MS = millis();
    }
}

float CNCTemperature::getTemperature(byte sensorNumber = 1) {
    return this->_lastTemperatures[sensorNumber - 1];
}

CNCTemperature::CNCTemperature(byte sensorPin, byte numberOfSensors = 1, uint32_t interval_MS = 5000) {
    this->_sensorPin = sensorPin;
    this->_numberOfSensors = numberOfSensors;
    this->_interval_MS = interval_MS;

    memset(&this->_lastTemperatures, -999, this->_numberOfSensors);

    this->_oneWire = OneWire(this->_sensorPin); 
    this->_sensors = DallasTemperature(&this->_oneWire);
}

CNCTemperature::~CNCTemperature() {}

#endif