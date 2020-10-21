#ifdef USE_RADIO 
/**
 *  Checking the radio connection
 */
uint32_t timeLastNRF24Check = 0;
void checkRadioConnection() {
    // Check NRF24 State every 10 seconds
    if (millis() - timeLastNRF24Check > 10000)
    {
        if(!radio.radio->isChipConnected()) {
            #ifdef DEBUG
                Serial.println("NRF24 chip is not connected or broken");
                Serial.println("Restart Arduino");
            #endif
            statusLed.setState(LEDControl::STATE::NRF24Failure);
        }
        timeLastNRF24Check = millis();
    }
}
#endif

#ifdef USE_DISPLAY 
/**
 *  Display Management
 */
enum FONT
{
    small,
    medium,
    big,
    symbols
};

void setFont(FONT font = FONT::small) {
    if(font == FONT::symbols) {
        display.setFont(u8g2_font_unifont_t_symbols);
    } else if(font == FONT::big) {
        display.setFont(u8g2_font_ncenB14_tr);
    } else if(font == FONT::medium) {
        display.setFont(u8g2_font_ncenB12_tr);
    } else {
        display.setFont(u8g2_font_ncenB08_tr);
    }
}

int8_t _lastDisplayOnboardTemperature = -127;
void updateDisplayOnboardTemperature(int8_t temp, uint8_t x = 0, uint8_t y = 0) {
    if(_lastDisplayOnboardTemperature == temp) {
        return;
    }
    _lastDisplayOnboardTemperature = temp;
    setFont(FONT::medium);

    display.setCursor(x,y+14);
    if(temp == -127) {
        display.print("off");
    } else {
        display.print(temp);
        display.print(" C");
    }

    uint8_t xTile = x/8;
    uint8_t yTile = y / 8; 
    uint8_t xWidthTile = 6;
    uint8_t yHeightTile = 2;

    display.updateDisplayArea(xTile,yTile,xWidthTile,yHeightTile);
}

int8_t _lastDisplayExternalTemperature = -127;
void updateDisplayExternalTemperature(int8_t temp, uint8_t x = 50, uint8_t y = 0) {
    if(_lastDisplayExternalTemperature == temp) {
        return;
    }
    _lastDisplayExternalTemperature = temp;
    setFont(FONT::medium);

    display.setCursor(x,y+14);
    display.print("E: ");
    if(temp == -127) {
        display.print("off");
    } else {
        display.print(temp);
        display.print(" C");
    }
    

    uint8_t xTile = x/8;
    uint8_t yTile = y / 8; 
    uint8_t xWidthTile = 10;
    uint8_t yHeightTile = 2;

    display.updateDisplayArea(xTile,yTile,xWidthTile,yHeightTile);
}

uint8_t _lastDisplayFanSpeed = -1;
void updateDisplayFanSpeed(uint8_t speed, uint8_t x = 0, uint8_t y = 18) {
    if(_lastDisplayFanSpeed == speed) {
        return;
    }
    _lastDisplayFanSpeed = speed;

    setFont(FONT::medium);
    display.drawStr(x,y+14, "Fan: ");

    int startx = x+45;
    for(int i=0;i<7;i++) {
        if(speed > i) {
            display.drawBox(startx, y, 7, 14);
            startx += 10;
        } else {
            display.drawFrame(startx, y, 7, 14);
            startx += 10;
        }
    }

    uint8_t xTile = x/8;
    uint8_t yTile = y / 8; 
    uint8_t xWidthTile = 16;
    uint8_t yHeightTile = 2;

    display.updateDisplayArea(xTile,yTile,xWidthTile,yHeightTile);
}

void printAutosquareState(char label[], AUTOSQUARESTATE state, uint8_t x, uint8_t y, FONT font = FONT::small) {
    display.setCursor(x, y);
    setFont(font);    
    display.print(label);
    if(state == AUTOSQUARESTATE::off) {
        setFont(FONT::symbols);
        display.drawGlyph(x+20, y+2, 0x2715);
        setFont(font);
    }
    if(state == AUTOSQUARESTATE::undefined) {
        display.print("? ");
    } 
    if(state == AUTOSQUARESTATE::done) {
        setFont(FONT::symbols);
        display.drawGlyph(x+20, y-1, 0x2713);
        setFont(font);
    }
}

DATA_FOR_DISPLAY _lastDisplayDataForDisplayStates = { 0,0,0,0,0,0,0,0,0 };
void updateDisplayAutosquareState(DATA_FOR_DISPLAY *data, uint8_t x = 0, uint8_t y = 0, FONT font = FONT::small, uint8_t yTiles = 0, uint8_t spacing = 2) {
    if(_lastDisplayDataForDisplayStates.stateX1 == data->stateX1 
        && _lastDisplayDataForDisplayStates.stateX2 == data->stateX2
        && _lastDisplayDataForDisplayStates.stateY1 == data->stateY1
        && _lastDisplayDataForDisplayStates.stateY2 == data->stateY2
        && _lastDisplayDataForDisplayStates.stateZ1 == data->stateZ1
        && _lastDisplayDataForDisplayStates.stateZ2 == data->stateZ2
    ) {
        return;
    }
    _lastDisplayDataForDisplayStates = *data;
    uint16_t fontHeight = 8; // in Pixel

    if(font == FONT::medium) {
        fontHeight = 12;
    } else if(font == FONT::big) {
        fontHeight = 14;
    }
    
    display.setCursor(x, y);    
    printAutosquareState((char*)"X1: ",(AUTOSQUARESTATE) data->stateX1, x, y+fontHeight, font);
    printAutosquareState((char*)"Y1: ",(AUTOSQUARESTATE) data->stateY1, x+45, y+fontHeight, font);
    printAutosquareState((char*)"Z1: ",(AUTOSQUARESTATE) data->stateZ1, x+90, y+fontHeight, font);
    printAutosquareState((char*)"X2: ",(AUTOSQUARESTATE) data->stateX2, x, y + 2 * fontHeight + spacing, font);
    printAutosquareState((char*)"Y2: ",(AUTOSQUARESTATE) data->stateY2, x+45, y + 2 * fontHeight + spacing, font);
    printAutosquareState((char*)"Z2: ",(AUTOSQUARESTATE) data->stateZ2, x+90, y + 2 * fontHeight + spacing, font);
    uint8_t xTile = x / 8;
    uint8_t yTile = y / 8; 
    uint8_t xWidthTile = 16;
    uint8_t yHeightTile = yTiles;
    // If tiles are not defined, try to calculate
    if(yHeightTile == 0) {
        yHeightTile = (spacing + fontHeight * 2) / 8;
        if(((spacing + fontHeight * 2) % 8) > 2) yHeightTile++; // Add a tile if the height exceeds the calculated tiles    
    }

    display.updateDisplayArea(xTile,yTile,xWidthTile,yHeightTile);
}

void displaySetup() {
    display.begin();
    display.clearBuffer();
}

void display_128X32_1() {
    updateDisplayOnboardTemperature(dataForDisplay.onboardTemperature);
    updateDisplayExternalTemperature(dataForDisplay.externalTemperature);
    updateDisplayFanSpeed(dataForDisplay.fanSpeed);
}

void display_128X32_2() {
    updateDisplayAutosquareState(&dataForDisplay, 0, 0, FONT::small, 2, 0);
    updateDisplayOnboardTemperature(dataForDisplay.onboardTemperature, 0, 18);
    updateDisplayExternalTemperature(dataForDisplay.externalTemperature, 50, 18);
}

void display_128X32_3() {
    updateDisplayAutosquareState(&dataForDisplay, 0, 0, FONT::medium, 4);
}

void display_128X64_1() {
    updateDisplayOnboardTemperature(dataForDisplay.onboardTemperature);
    updateDisplayExternalTemperature(dataForDisplay.externalTemperature);
    updateDisplayFanSpeed(dataForDisplay.fanSpeed);
    updateDisplayAutosquareState(&dataForDisplay, 0, 40, FONT::small, 3);
}

uint32_t _lastDisplayUpdate = displayRefreshInterval_MS;
void displayUpdate(DISPLAYLAYOUT layout, bool force = false) {
    // Reset last states if display update is forced
    if (force) {
        _lastDisplayDataForDisplayStates = { 0,0,0,0,0,0,0,0,0 };
        _lastDisplayFanSpeed = -1;
        _lastDisplayExternalTemperature = -127;
        _lastDisplayOnboardTemperature = -127;
    }

    if (millis() - _lastDisplayUpdate > displayRefreshInterval_MS || force) {
        display.clearBuffer();
        if(layout == DISPLAYLAYOUT::Layout_128X32_Fan_and_Temp) {
            display_128X32_1();
        }
        if(layout == DISPLAYLAYOUT::Layout_128X32_Temp_and_Autosquare) {
            display_128X32_2();
        }
        if(layout == DISPLAYLAYOUT::Layout_128X32_Autosquare) {
            display_128X32_3();
        }
        if(layout == DISPLAYLAYOUT::Layout_128X64_1) {
            display_128X64_1();
        }        
        
        _lastDisplayUpdate = millis();
    }    
}

#endif
