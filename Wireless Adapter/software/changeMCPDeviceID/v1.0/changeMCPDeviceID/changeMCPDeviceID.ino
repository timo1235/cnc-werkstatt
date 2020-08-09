/*
  Writing programmable device id (0-7)
    Because of critical timing of LDAC latch during the address write and read, 
    the library use software I2C master library just for address read and writing.
    Included modified SoftI2cMaster library is required.
    Original library is from fat16lib , http://forums.adafruit.com/viewtopic.php?f=25&t=13722

  I2C CLOCK pin (SCL) need to connected to digital pin 2.
  I2C DATA pin (SDA) need to connected to digital pin 3.
  LDAC pin need to be connected to digital pin 8.

  If you are using new chip, Device ID is 0.
  If you don't know current device ID, just run this scketch and check the serial monitor.
  Once you get current Device ID, put proper current device ID in writeAddress() command.
*/

#include "SoftI2cMaster.h"

/*
  Change the pin, if you connect them other pins
*/
#define SCL_PIN 21
#define SDA_PIN 20
#define LDACpin 6

SoftI2cMaster i2c;
#include <Wire.h>

void setup()
{
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH);
  delay(500);
  Serial.begin(9600);  // initialize serial interface for readAddress()
  i2c.init(SCL_PIN, SDA_PIN); // initialize i2c interface
  pinMode(LDACpin, OUTPUT); // Set LDACpin to OUTPUT
  writeAddress(0,1);  // Write new address (current Device ID, new Device ID)
  delay(100); // wait for EEPROM writing 
  readAddress(); // Read current Device ID 
} 

void loop()
{
} 


void readAddress() {
byte count = 0;
int address = 0;
    Wire.begin();
    for (byte i = 8; i < 120; i++)
    {
        Wire.beginTransmission (i);
        if (Wire.endTransmission () == 0)
        {
            address = i;
            delay (1); // maybe unneeded?
        } // end of good response
    } // end of for loop
    if(address == 97) {
      Serial.println("Adresse erfolgreich geändert!");
    } else {
      Serial.println("Adresse konnte nicht geändert werden!");
    }
}

void writeAddress(int oldAddress, int newAddress) {
  digitalWrite(LDACpin, HIGH);
  i2c.start(0B11000000 | (oldAddress << 1));
  i2c.ldacwrite(0B01100001 | (oldAddress << 2), LDACpin); // modified command for LDAC pin latch
  i2c.write(0B01100010 | (newAddress << 2));
  i2c.write(0B01100011 | (newAddress << 2));
  i2c.stop();
}
