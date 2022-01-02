# Table Router Shield Sketch
This sketch is intended to work with the table router shield on an ESP32

## Changelog
### v2.0
* big rewrite, changed most of the functions
* added possibility for controlling a spindle with fu
  * added control for the 0-10V analog output
  * added functionalities for the spindel start/stop button
* fixed rounding issues in step calculation
* no more automatically driving from endstop, change to manual mode instead

### v1.0
* Initial release
