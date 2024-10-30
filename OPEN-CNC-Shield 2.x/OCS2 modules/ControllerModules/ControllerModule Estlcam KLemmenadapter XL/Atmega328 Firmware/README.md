# ControllerModule Estlcam Klemmenadapter
This firmware is for the ControllerModule Estlcam Klemmenadapter, specifically for the ATmega328 microcontroller on the ControllerModule. Designed for integration with Estlcam via I2C communication, this firmware facilitates the precise and reliable transfer of handwheel signals from the Open CNC Shield 2 to the Estlcam software, enabling enhanced manual control over CNC machine functions.

The controller module interprets inputs from various components (joystick, potentiometers, control buttons) on the handwheel and translates them into actionable data for the CNC system. With features including error handling, and input validation this code supports secure and smooth communication with Estlcam, helping ensure responsive CNC control.

## Key Features:
### I2C Communication:
Establishes and maintains a stable I2C connection with Estlcam.
Uses a modular data frame structure for bidirectional communication between the handwheel and Estlcam.
### Input Handling:
- Reads analog values from joystick (X, Y, Z axes) and potentiometers (feed rate and rotation speed).
- Processes digital inputs for programm start, motor start and "OK" button
### Error Detection and Handling:
- Implements error thresholds to detect irregular analog input signals
- Automatically adjusts for errors in joystick and potentiometer values 
### Frame and Checksum Management:
- Manages frame counts and calculates checksums for each data frame to ensure data integrity.