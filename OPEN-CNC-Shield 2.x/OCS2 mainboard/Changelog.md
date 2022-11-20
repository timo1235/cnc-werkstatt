# OCS2 mainboard
## Changelog
### 2.08
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