/**
 * Library for CNC related controls
 * Version 1.0
 * find out more: blog.altholtmann.com
 * by Timo Altholtmann
 **/


#ifndef cnclibrary_h
#define cnclibrary_h

#include <Arduino.h>
#include "cnclibrary_ledstate.h"
#include "cnclibrary_radio.h"
#include "cnclibrary_temperature.h"


enum AUTOSQUARESTATE
{
    undefined,
    done,
    off
};

enum DISPLAYLAYOUT {
    Layout_128X32_Fan_and_Temp,
    Layout_128X32_Temp_and_Autosquare,
    Layout_128X32_Autosquare,
    Layout_128X64_1
};

// For Communication between the Wireless Adapter and the controller arduino
// uint16_t : 2Bytes
// unsigned : 1 Bit each
typedef struct {
  float onboardTemperature;
  float externalTemperature;
  uint8_t fanSpeed;
  AUTOSQUARESTATE x1;
  AUTOSQUARESTATE x2;
  AUTOSQUARESTATE y1;
  AUTOSQUARESTATE y2;
  AUTOSQUARESTATE z1;
  AUTOSQUARESTATE z2;
} DATA_FROM_CONTROLLER;

typedef struct {
    float onboardTemperature;
    float externalTemperature;
    unsigned fanSpeed : 3;
    unsigned stateX1 : 2;
    unsigned stateX2 : 2;
    unsigned stateY1 : 2;
    unsigned stateY2 : 2;
    unsigned stateZ1 : 2;
    unsigned stateZ2 : 2;
} DATA_FOR_DISPLAY;

#endif