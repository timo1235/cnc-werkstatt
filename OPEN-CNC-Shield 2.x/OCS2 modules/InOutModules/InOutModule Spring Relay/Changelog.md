# InOutModule Spring Relay

## Changelog

### 1.08

- Replaced 1k 500mW resistors with 1k 1W resistors

### 1.07

- Added own resistors for all COM1 and GND lines to the slide switches
- Removed resistors arrays and added single resistors instead

### 1.06

- Corrected position of switches
- Corrected orientation of switches
- Added 1k resistor for all COM1 lines to the slide switches to prevent short circuit in case of broken slide switches
- Added 1k resistor for all GND lines to the slide switches to prevent short circuit in case of broken slide switches

### 1.05

- Changed spring terminals footprint to universal footprint
- Removed input type jumper for all inputs
- Added input type switch for all inputs
- Replaced relay from 5A swiching current to 10A switching current
- Changed input type switch connection from 5V to COM1. NPN sensors should now work as expected
- Changed OCS2 connector to 20pin version to prevent wrong positioning

### 1.04

- Replaced toshiba optocoupler with everlight
- Replaced resistor of out1-4 leds from 470 ohm to 2k ohm

### 1.03

- Fixed wrong selection setting of IN1. The jumper was reversed.
