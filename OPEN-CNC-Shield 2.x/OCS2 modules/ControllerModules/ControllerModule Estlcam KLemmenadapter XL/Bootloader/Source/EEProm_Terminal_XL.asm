	.db		1	;Config Data Version 1 (MUST BE 1)
	.db		1	;External 24MHz Clock: 
					;0= No external clock / pins are used for something else and bootloader MUST NOT try to start the external clock.
					;1= External crystal or resonator may be available / bootloader should try to enable it and if no external clock is found switch to internal again.
	.db		1	;1= Galvanically isolated USB interface (just for information)
	.db     1	;1= Y axis autosquare possible (LPT Adapter always has this capability / Terminal Adapter only if both "Step YL" and "Step YR" signals are made accessible to the user)
	.db		1	;1= Handwheel pendant connector available
	
	.db		16 	;Number of digital Inputs (For LPT Adapter: there are always at least 5 inputs @ LPT connector pins 10 / 11 / 12 / 13 / 15)
	;		1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16     ;Digital input description table
	.db		2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2      ;Input  Type: 0=Not available / 1=TTL / 2=NPN / 3=PNP (More types may be added - please inform me if you add something different)

	.db		1	;Frequency / pulse counter Input available: 0=Not available / 1=TTL (More types may be added - please inform me if you add something different)
	
	.db		7	;Number of Analog Inputs (max 7 for now)
	;		1      2      3      4      5      6      7      8      9      10       ;Analog input description table
	.db		1,     1,     1,     2,     2,     3,     3,     0,     0,     0      ;Sensor Type 0=Not available / 1=Resistance / 2=Current 0-20mA / 3=Voltage 0-10V (More types may be added - please inform me if you add something different)
	.dw		5000,  5000,  5000,  5000,  5000,  5000,  5000,  0,     0,     0		;Value of Resistor "R_R": Multiply the actual value by 5 (1k Ohm Resistor = 5000)
	.dw		50000, 50000, 50000, 50000, 50000, 50000, 50000, 0,     0,     0		;Value of Resistor "R_IRU": Multiply the actual value by 5 (10k Ohm Resistor = 50000)
	.dw		25500, 25500, 25500, 25500, 25500, 25500, 25500, 0,     0,     0		;Value of Resistor "R_IU": Multiply the actual value by 5 (5.1k Ohm Resistor = 25500)
	.dw		1200,  1200,  1200,  1200,  1200,  1200,  1200,  0,     0,     0		;Value of Resistor "R_I": Multiply the actual value by 5 (240 Ohm Resistor = 1200)
    	
	;The following values are the voltage reference voltages multiplied by 1000
	;There may be future use for high precision calibration - but for now it is unused.
	.dw		1024	;VRef internal 1.024V
	.dw		2048	;VRef internal 2.048V
	.dw		4096	;VRef internal 4.096V
	.dw		2500	;VRef internal 2.500V
	.dw		5000	;VDD
	.dw		5000	;VREFA external D7
	
	.db		10	;Number of available digital outputs (max 20)
	;Any pin not used for something else can be used as output.
	;You can e.g. decide to sacrifice some input pins in favour of outputs.
	;And you can change the order of the pre defined output pins to ease PCB routing
	;		1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    ;Digital output description table
	.db		0x43, 0x44, 0x45, 0x46, 0x47, 0x50, 0x51, 0x54, 0x55, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  ;Output Port and Pin
																																	;Upper 4 Bits: 0-6= Port A-G
																																	;Lower 4 Bits: 0-7= Pins 0-7
																																	;e.g. "0x43"
																																	;         ^3= Pin 3
																																	;        ^4 = Port E
																																	;0xFF= Not available
	.db     2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0     ;Output type *See output types list below
	.db		0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0xF4, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0     ;Output max current mA **See 8Bit Number values																																	
	.db		0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0xF1, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0     ;Output max voltage V **See 8Bit Number values
																																	
	.db     2		;Spindle output type *See output types list below
	.db		0xF4	;Spindle output max current mA **See 8Bit Number values
	.db		0xF1	;Spindle output max voltage V **See 8Bit Number values
	.db     1		;PWM output type  *See output types list below
	.db		0x52	;PWM output max current mA **See 8Bit Number values
	.db		0x50	;PWM output max voltage V **See 8Bit Number values
	.db		8		;MMS output type  *See output types list below
	.db		0x8A	;MMS output max current mA **See 8Bit Number values
	.db		0xF1	;MMS output max voltage V **See 8Bit Number values
	.db		1		;RPM Output 0=Not available; 1=0-10V adjustable / 2=0-5V fixed / 3=0-10V fixed

	;*Output types:
	;0=Not available
	;1=TTL 5V
	;2=Relay driver
	;3=Relay NO
	;4=Relay NC
	;5=Relay NO/NC
	;6=Power Mosfet NPN
	;7=Power Mosfet PNP
	;8=Current limited solenoid driver output
	;More types may be added - please inform me if you add something different

	;**8Bit number values
	;The upper 4 bits specify a multiplicant ranging from 0 to 15
	;The lower 4 bits specify a multiplier where the values from 0 to 15 stand for multipliers of: 1  2  4  8  10  20  40  80  100  200  400  800  1000  2000  4000  8000
	;This way values from 0 to 128000 can be represented by 8 bit (with large but reasonable gaps in between)
	;Example: "0xF4"
	;Upper 4 Bits "F" = 15
	;Lower 4 Bits "4" = 10
	;The value is 15 x 10 = 150																																
	
	;Optional: if you like I can provide you with custom IDs to identify your product.
	;By entering the IDs here Estlcam is able to display your company and product name to the user
	;And provide links to your shop and user manual
	;Also, if you e.g. bundle the product with a certain machine I can add one or more presets that the user then can choose from 
	;This way the user can just select the appropriate preset and skip manual configuration.
	.dw		1	;Manufacturer ID 0=Any
	.dw		1	;Product ID 
	.dw		1	;URL shop ID 
	.dw		1	;URL user manual ID
	.dw		0	;Configuration Presets ID
	
	;################################################################################################
	;#                                                                                              #
	;# DO NOT WRITE ADDITIONAL DATA TO THE UNUSED EEPROM MEMORY SPACE WITHOUT CONSULTING ME FIRST!  #
	;# IT MAY RENDER YOUR PRODUCT UNUSABLE IF LATER ADDED EEPROM VARIABLES CONFLICT WITH YOUR DATA! #
	;# I WILL TRY TO KEEP THE UPPER 256 BYTES FREE FOR OTHER USE - BUT I CANNOT GUARANTEE THIS      #
	;# IF YOU NEED EEPROM FOR A FIRMWARE PORT FILL FROM THE END AND KEEP THE BEGINNING INTACT       #
	;#                                                                                              #
	;################################################################################################