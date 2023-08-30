# ESP32 Panel PCB
## Changelog
### 1.07
- Changed "powerbank charging usb a" port VCC from `5V_Extern` to `+5V_before_switch` to prevent backflow further
### 1.06
- Added usb c port at the front to power the pcb and charge the powerbank
- Moved the RJ45 connector a bit to the right to make room for the usb c port
- Removed the powerbank jumper and added a mosfet to automatically select the correct power source
- Added a diode to the esp32 5V connection to prevent backflow+
- Changed I2C display power from 5V to 3.3V
### 1.05
- Removed menu button GND connection and added menu button 3.3V connection
### 1.04
- initial