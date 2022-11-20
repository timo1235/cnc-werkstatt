# Table Router Shield

## The design is still experimental and currently being tested!!

## Changelog
### 1.6
* Changed input volatge from 12V to 12V-32V
* Changed input led resistor from 680ohm to 1800ohm
* Added OR-Gate to increase voltage of PULL, DIR and ENA to 5V
* Changed 10K single resistors to resistor array

### 1.5 
* Added Pullup resistors to ESP32 input-only pins 34,35,36,39

### 1.4
* Changed some labels
* Changed display powering. Now its powered also if only USB is available

### 1.3
* Fixed screw terminal spacing
* Changed voltage for the rotation speed poti to 3.3V
* Fixed voltage regulator design

### 1.2
* Changed screw pads to be connected to GND
* Added screw terminal for direct motor connection to the onboard driver
* Added screw terminal for spindle on off button
* Chaned 5V power supply from LDO to dc-to-dc converter
* Added self ressting fuses to 5V and 3.3V net
* Added diodes to prevent the esp from powering 12V net
* Power LEDs directly from esp32 3.3V
* Changed resistor of analog signal circuit, so that 0-10V can be achieved
* Fixed spindle on/off LED
* Removed 5V/10V jumper, since the esp32 would output only 0-3.3V instead of 0-5V. Furthermore the ouput of 0-10V should be sufficient for all spindles

### 1.1
* Added leds for
    * inputs
    * 12V power indicator
    * 5V power indicator
* Changed main processor from Arduino Nano to ESP32 Devkit V1 because of dual core abilities

### 1.0
* Initial design
