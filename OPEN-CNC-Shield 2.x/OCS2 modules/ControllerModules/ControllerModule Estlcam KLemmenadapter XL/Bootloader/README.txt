Recommended programmer hardware: Microchip / Atmel ICE
Recommended programmer software: AVRDUDE (included in this folder)

################################################################################################################
If you manufacture one of the predefined products without modification you can use the corresponding batch file to program the device:
		Terminal_XL.bat
		Terminal_M.bat
		Terminal_Light.bat
		LPT_XL.bat
		LPT_Light.bat
	After this the board is ready to use.

################################################################################################################
If you manufacture a customized product:

	Step 1: Program Flash Memory with the correct bootloader:
		Bootloader_Terminal.hex: if it is a variation of the "Terminal Adapter"
		Bootloader_LPT.hex: if it is a variation of the "LPT Parallelport Adapter"
		
	Step 2: Set Fuses to
		BOOTSIZE: 0x02	(1024 Byte Bootloader)
		WDTCFG:   0x09  (Watchdog 2 seconds)

	Step 3: Program EEProm memory with a .eep file describing your products features:
		- Start "EEProm_Tool.exe" in the "EEProm_Tool\bin\Debug" folder
		- Customize the settings (have a look at the Preset Buttons at the right bottom to get started)
		- Create a ".epp" file for EEProm programming e.g. with AVRDUDE by pressing the "Save EEP File" Button
		- Or alternatively upload the Configuration by pressing the "Save EEProm Configuration to Board" Button directly with EEProm_Tool.
		(Note: EEProm_Tool is very basic and does not perform input validation - entering e.g. text in number fields will crash the application.)		
	
	
################################################################################################################
If you like to port a different control system like e.g. "GRBL" to this hardware please keep the bootloader compatible so users can switch easily.
If bootloader modifications cannot be avoided please add a switch to the board so the user can change between "Estlcam mode" and your modification.
The bootloader section size is 1024 bytes with about 400 bytes used by Estlcam, so there is space left for modifications.

Please consider that Estlcam needs the configuration data stored in the EEProm memory.
If your firmware uses EEProm please provide the customer with a tool to restore the EEProm to its original settings (you can modify "EEProm_Tool.exe" for this)


################################################################################################################
Bootloader protocol:

The bootloader starts automatically during power up and remains active until the exit command is sent.
Connect to it using 1000000 Baud / 8 Bit / 1 Stopbit / No parity

Then you can send byte commands - there are just 4:

	0: Exits the bootloader:
		- As the bootloader remains active until this command is sent any control software needs to begin communication by sending a 0 byte.
	
	1: Sends a 32 Byte long identification string:
		- "Estl_Boot 1.01 LPT Adapter 00005" if the hardware is a LPT / Parallelport Adapter
		- "Estl_Boot 1.01     Adapter 00006" if the hardware is a Terminal Adapter
		- The string has no termination character or length field - just 32 bytes of text

	2: Writes one page (512 bytes) of Flash or EEProm memory:
		- Follow the command by the 32 byte identification string (see previous command)
			- If this string is not sent or incorrect the bootloader will not write the memory page to prevent memory corruption by randomly received data.
		- Send one byte specifying the number of the memory page you like to program:
			- Application Section Flash Memory starts at page 2 (Memory Adresses 1024 to 1535) and ends at page 127 (Memory Adresses 65024 to 65535)
			- Sending page 0 will write to the EEProm memory instead of Flash
			- Page 1 is an invalid address as it is within the write protected Bootloader Section.
		- Wait for the bootloader to respond with one byte with the value 1
		- Send the 512 byte memory page
		- Wait for the bootloader to respond with one byte with the value 2	(in case of EEProm writes this takes 5-6 seconds)

	3: Reads one page (512 bytes) of Memory
		- Followed by one byte specifying the number of the memory page you like to read:
			- Application Section Flash Memory starts at page 2 (Memory Adresses 1024 to 1535) and ends at page 127 (Memory Adresses 65024 to 65535)
			- Sending page 0 will read the EEProm memory instead of Flash
			- Sending page 1 will read the IO Memory space from 0x1000 to 0x11FF containing e.g. Lock / Fuse / Userrow and Sigrow data (see Datasheet).