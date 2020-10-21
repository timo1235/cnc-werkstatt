#ifndef cnclibrary_radio_h
#define cnclibrary_radio_h

// includes
#include <Arduino.h>
#include <RF24.h>

/**
 * Common structs needed for wireless commnunication
 **/
// Max size of this struct is 32 bytes - NRF24L01 buffer limit
// uint16_t : 2Bytes
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

enum ROLE {
    panel,
    adapter
};

class CNCRadio {
private:
    ROLE role;
    uint8_t addresses[2][6] = {"No1","No2"};
public:
    RF24 *radio;
    CNCRadio(uint16_t cePin, uint16_t csPin, ROLE role);
    ~CNCRadio();
    void setup();
};



/** 
 * Constructor of the CNCRadio class.
 * @param cePin 
 * @param csPin
 * @param role Specify if the radio is a adapter(sender) or panel(receiver)
 */
CNCRadio::CNCRadio(uint16_t cePin, uint16_t csPin, ROLE role = ROLE::adapter) {
    this->radio = new RF24(cePin, csPin);   // nRF24L01 (CE, CSN)
    this->role = role;
}

/** 
 * Destructor of the CNCRadio class.
 */
CNCRadio::~CNCRadio() {}

void CNCRadio::setup() {
    this->radio->begin();
    this->radio->setDataRate(RF24_2MBPS);
    this->radio->setPayloadSize(sizeof(DATA_TO_CONTROL));
    this->radio->enableAckPayload();   // Allow optional ack payloads
    // radio.setPALevel(RF24_PA_LOW);
    // radio.setRetries(0,15);
    if(this->role == ROLE::panel) {
        this->radio->enableDynamicPayloads();                // Ack payloads are dynamic payloads
        this->radio->openWritingPipe(addresses[0]);
        this->radio->openReadingPipe(1,addresses[1]);
        this->radio->startListening();
    } else if (this->role == ROLE::adapter) {
        this->radio->setAutoAck(1);
        // radio.enableDynamicPayloads(); // Ack payloads are dynamic payloads
        this->radio->openWritingPipe(addresses[1]);
        this->radio->openReadingPipe(1, addresses[0]);        
    }
}

#endif