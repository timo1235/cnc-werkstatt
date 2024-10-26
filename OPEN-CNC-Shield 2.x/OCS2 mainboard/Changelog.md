# OCS2 mainboard

## Changelog

### 2.14 
- toDo: Add capacitors to ICs

### 2.13

- Changed ENA connections
  - Route ENA from ControllerModule through the octal buffer to the drivers, to ensure 5V logic level, regardless of the controller module
  - Changed resistor of JP3 jumper to ENA from 1k to 4.7k
- Added 2.4k resistors to all autosquare-button nets
- Removed analog switches for analog expander
  - Added mosfet to cut off the power for the analog expander when no ESP32 is installed
- Removed 74HCT541 octal buffer for DIR ouputs from controllermodule
  - Added two multiplexer to select whether the DIR signal comes from the controllermodule or from the ESP32, default is from controllermodule
  - Removed 74HCT541 DIR buffer between driver and multiplexer

### 2.12

- Fixed wrong wire connections due to naming issues with the software
- added two BAV70DW schottky gates from outputs 1-4 and removed an or gate 74HCT32 instead

### 2.11

- removed two BAV70DW schottky gates from outputs 1-4 and added an or gate 74HCT32 instead
- seperated 5V to several nets to protect against short circuits
  - seperate 5V net for all ICs
  - seperate 5V net for the drivers
- Moved DAC 2k4 resistors from DAC to the analog multiplexer

### 2.10 - 23.08.2023

- Added two analog switches to cut off the analog signals from analog expander when no ESP32 is installed
- Removed ESP_D32 from pinout because it controls now the analog switches
- Changed resistance for analog outputs from 20k to 2k4

### 2.09 20.08.2023

- Replaced manufacturer of CN5, CN6, CN7, CN8
- Added 74HCT541 octal buffer for DIR ouputs from controllermodule to cut the controller DIR connection while autosquaring
- Removed ESP_D27 from Pinout because it controls now the octal buffer enable state for the DIR outputs
- Added Thyristor Surge Suppressors(TSS) for overvoltage protection of ICs
- Changed resistance for analog outputs from 2k4 to 20k

### 2.08 - 15.11.2022

- J3 jumper is now connected to V_External instead of VIN
- Removed GND connection to DAC reset pin

### 2.07

- Added jumper from VIN to VMOT, so that an extra power source is not needed for onboard drivers
- Added 2.4k Ohm resistors to handwheel inputs. So its possible to have a wired handwheel as well as a wireless handwheel
- Moved kondensator, which was placed under the controller module, because he touched the pads of the controller module

### 2.06

- Added 2.4k Ohm resistors in series with analog outputs of the BU2506 chip. This makes it possible to also connect wired analog inputs at joystick/feedrate/rotation speed pins
- Changed 5V dc converter to support 5V 3A
- Increased trace width for 5V
- Changed pinout of PanelModule connector, changed from i2c to uart for panel communication
  - Removed ESP_SDA and ESP_SCL
  - Added ESP_TX and ESP_RX
- Changed pinout for esp32
  - now SDA and SCL are used for onboard chip communication instead of SDA2 and SCL2
  - SDA2 and SCL2 pins are now used as TX_2 and RX_2 for communication with the panel module

### 2.05

- reset pin of BU2506FV is now tied to GND
- added 2k2 resistors in series to analog outputs of BU2506FV. Otherwise it was not possible to use a handwheel directly with the ocs2 pinout
