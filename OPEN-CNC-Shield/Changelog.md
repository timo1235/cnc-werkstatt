# OPEN-CNC-Shield
## Changelog
### 2.0
* Changed complete design for smd build
* Replaced the arduino pro mini for estlcam with the original Mega2560 because pro mini support in estlcam is needy
* Added second onboard temp sensor near the voltage regulators
* Completely removed 12V power net

### 1.9
* Increased GND spoke width from power supply to the shield
  
### 1.8
* Changed Resistor value of R20 from 10K to 1K
  
### 1.7
* Increased size of connection holes
* Added diode for second arduino
* Second arduino is now also powered via 5V instead of 12V
* Added pads for dsub connector

### 1.6
* Added jumper configuration for default ena state
* Fixed footprint of DS18B20 for easier soldering
* Changed headers for the motor drivers to be in the same grid
* Changed GND pads for easier soldering

### 1.5
* Added jumper configuration for selecting the voltage at an input(COM3, COM4)

### 1.4
* Added 1k ohm resistor to pwm net
* Added gnd to relais terminal
* Removed second arduino diode on Vin
* Changed position of leds for more space

### 1.3
* Changed label of buck converter
* Changed 7812 linear voltage regulator to buck converter to avoid heat
* Increased trace width for 5V and 12V net to support higher current
* Changed SB240 to SB340
* Switched position of OUT7 and OUT8
* Correct label of OUT7
* Increased pad for 1.2k resistors
* Fixed footprint of mp1854
* Changed label of J14 to JP20
* Added terminal for PWM and Spindle on/off relais
* Changed label of spindel pwm terminal to inverter analog
* Removed pwm jumper
* Removed analog jumper

### 1.2
* Changed 7805 linear voltage regulator to buck converter to avoid heat
