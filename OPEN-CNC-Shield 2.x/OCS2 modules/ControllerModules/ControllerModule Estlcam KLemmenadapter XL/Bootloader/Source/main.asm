;##########################################################################################################################
;Select the "LPT Adapter" or "Terminal Adapter" Solution configuration depending on hardware

;##########################################################################################################################
;Fuse configuration:
;Bootsize: 0x02 / Bootloader size 1024 bytes
;WDTCFG:   0x09 / Watchdog 2 seconds

;##########################################################################################################################
.equ	Clock		= 24000000
.equ	Baud		= 1000000
.equ	USB_In		= USART0_RXDATAL
.equ	USB_Out		= USART0_TXDATAL
.equ	USB_State	= USART0_STATUS
.equ	USB_Baud	= USART0_BAUD
.equ	USB_CB		= USART0_CTRLB
.equ	USB_CC		= USART0_CTRLC
.equ	USB_Mux		= PORTMUX_USARTROUTEA
.equ	USB_Dir		= PORTA_DIRSET
.equ	USB_TX_Pin	= 4
;						  R0		;Reserved for SPM
;						  R1		;Reserved for SPM
.def	T1				= R16		;Multi purpose temporary register 1
.def	T2				= R17		;Multi purpose temporary register 2
.def	Count			= R18		;Counter register
.def	MEM_Page		= R19		;Current memory page
.def	R_Zero			= R20		;Constant 0 Value
.def	R_CCP			= R21		;Constant CPU_CCP_IOREG_GC Value

.include "Macros.asm"

;Reset 13.07.24##########################################################################################################################
RESET:
	CLI												;Disable interrupts
	WW		CPU_SPL,RAMEND							;Set stack pointer
	LDI		R_Zero,0								;Load 0 Constant to R_Zero
	LDI		R_CCP,CPU_CCP_IOREG_GC					;Load CPU_CCP_IOREG_GC Constant to R_CCP

;Init Clock 13.07.24##########################################################################################################################
	LR		T1,EEPROM_START+1										;Should we try to start the external oscillator? Load EEProm clock configuration byte @EEPROM_START+1.
	CPI		T1,255 BREQ OSC_INT_CLK									;If 255 (default value of unwritten EEProm cells) skip external clock startup and go to internal clock startup (e.g. in case other things than an oscillator are connected to the XTAL pins)

			CCP_Write CLKCTRL_XOSCHFCTRLA,0b10111001				;Enable external HF oscillator
			LXW 2048												;Set timeout in X Register pair to 2048 tries
OSC_W_X24:	SBIW X,1 BREQ OSC_INT_CLK								;If timeout passed then no external clock available -> go to internal clock startup		
			LR T1,CLKCTRL_MCLKSTATUS								;Test if clock is stabilized
			SBRS T1,4	RJMP OSC_W_X24								;No? Keep looping

			CCP_Write CLKCTRL_MCLKCTRLA,CLKCTRL_CLKSEL_EXTCLK_GC	;Switch to external HF oscillator / No need to wait for the switch to complete - makes no difference if the next instructions still execute on the old clock
			RJMP OSC_INIT_END

OSC_INT_CLK:												;In case no external HF oscillator is availabe switch to the internal 24MHz clock
	CCP_Write		CLKCTRL_OSCHFCTRLA,0b10100100			;Set internal OSC to 24MHz
	Wait_BIT_SET	CLKCTRL_MCLKSTATUS,1					;Wait for OSC stabilization
	CCP_Write		CLKCTRL_MCLKCTRLA,0						;Switch to internal OSC / No need to wait for the switch to complete

OSC_INIT_END:

;Init USART 13.07.24##########################################################################################################################
	WV USB_MUX,		0b00000001								;Alternative pin location USART0 PA4/PA5
	WW USB_BAUD,	(64*Clock)/(16*Baud)					;Baudrate...
	WV USB_CC,		0b00000011								;8 Bit / 1 Stopbit...
	WV USB_DIR,		(1<<USB_TX_Pin)							;TX Pin as output....
	WV USB_CB,		0b11000000								;Enable transmitter and receiver

;Main Loop 13.07.24##########################################################################################################################
Main:
	RCALL		RX_Wait	
	CPI			T1,4	BRSH Main			;Check if command is valid - if not go back to main loop
	LZW			JT							;Load Jump table address to Z
	ADD			ZL,T1						;Add command to address / Ignore carry - no risk of segment crossing here
	IJMP 									;Jump into table
JT:	RJMP		0x0200						;0 Jump to App Section / leave bootloader
	RJMP		Identify					;1 Jump to Identify command
	RJMP		Write_Page					;2 Jump to Write Page command
	RJMP		Read_Page					;3 Jump to Read Page command

;##########################################################################################################################
Identify:									;Send identification string
				LZW		ID_Start*2	
IL:				LPM		T2,Z+ 	
				RCALL	TX_Wait		
				CPI		ZL,Low(ID_End*2)	BRNE IL	
RJMP Main

;##########################################################################################################################
Write_Page:
	LZW		ID_Start*2							;Protect against accidential writes by verifying against identification string
VL:	LPM		T2,Z+ 	
	RCALL	RX_Wait	
	CP		T1,T2 BRNE Main						;Back to main loop if ID not matching
	CPI		ZL,Low(ID_End*2) BRNE VL

	RCALL	Get_Z_Addr							;Receive the page to write and calculate Z pointer address
	
	CPI		MEM_Page,0	BRNE WFL				;EEprom or Flash?

WEE:	LDI	T2,0x13	RCALL SPM_Write				;Page = 0 -> EEProm -> Erase and write command
		RJMP	RB

WFL:	LDI	T2,0x08	RCALL SPM_Write				;Page <> 0 -> Flash -> Page erase Command								
		SPM										;Page Erase Execute
		Wait_BIT_CLR NVMCTRL_Status,0			;Wait completion
		LDI	T2,0x00	RCALL SPM_Write				;NOP Command
		LDI	T2,0x02	RCALL SPM_Write				;Page Write Command
	
RB:	WV		USB_Out,1							;Signal ready to receive
	LDI		Count,0								;Read 256 x 2 bytes...
	LXW	Buffer									;AVR64DB programs slowly - data arrives faster than programming is possible -> read data first, then program
LB:		RCALL	RX_Wait	ST X+,T1					
		RCALL	RX_Wait	ST X+,T1
		DEC		Count	BRNE LB

	LDI		Count,0								;Program 256 x 2 bytes...
	LXW		Buffer
WL:		WDR										;Write Loop / EEProm programming takes about 5 seconds - prevent the watchdog from triggering in the meantime.
		LD		R0,X+							;Load from Buffer to R0 and R1
		LD		R1,X+
		CPI		MEM_Page,0 BREQ PEE				;Flash or EEProm?
PFL:		SPM									;Store to Flash
			ADIW	Z,2							;Inc Z
			RJMP PN
PEE:		ST	Z+,R0							;Store to EEProm
			ST	Z+,R1
PN:	DEC Count BRNE WL
			
	Wait_BIT_CLR NVMCTRL_Status,0				;Wait till complete
	LDI	T2,0x00	RCALL SPM_Write					;Send NOP command				
	WV	USB_Out,2								;Signal ready for next command
RJMP Main

;##########################################################################################################################
Read_Page:
	RCALL	Get_Z_Addr							;Receive the page to read and calculate Z pointer address			
	LDI		Count,0		
RL:		RCALL CMS
		RCALL CMS
	DEC Count BRNE RL
RJMP Main

;##########################################################################################################################
Get_Z_Addr:
	RCALL	RX_Wait							;Receive the page to read
	MOV		MEM_Page,T1						;Save page to MEM_Page register
	LDI		ZL,0							;ZL is always 0
		
		LDI		ZH,0x10						;Assume Sigrow should be targeted (Mem_Page = 1)
		CPI		MEM_Page,1					;Was the assumption correct?
		BREQ	GE							;Goto End

		LDI		ZH,High(EEPROM_START)		;Assume EEProm should be targeted (Mem_Page = 0)
		BRLO	GE							;Was the assumption correct? Goto End

		MOV		ZH,MEM_Page					;Target Flash ZH= MEM_Page * 2
		LSL		ZH							;Multiply by left shifting		
GE:	RET

;##########################################################################################################################
CMS:	;Conditional Memory Send / Sends one EEProm or Sigrow byte if MEM_Page is 0 or 1, otherwise one Flash byte
		LD		T2,Z+						;Assume EEProm or Sigrow should be read
		CPI		MEM_Page,2					;Was the assumption correct?
		BRLO    CS							;Goto Send
		LPM		T2,Z+						;Else Load Flash
CS:		RCALL TX_Wait						;Send the byte in T2
RET

;##########################################################################################################################
RX_Wait:		;Received byte in T1
	WDR							;Reset Watchdog
	LR		T1,USB_State		;Read USB Status Register
	SBRS	T1,7 RJMP RX_Wait	;Continue loop until Receive Flag set
	LR		T1,USB_In	
RET

;##########################################################################################################################
TX_Wait:		;Byte to send in T2 / Register T1 is changed
	Wait_BIT_SET	USB_State,5
	WR	USB_Out,T2
RET

;##########################################################################################################################
;Performs a write to the configuration change protected NVMCTRL_CTRLA register
;T2 = Value to write
;T1 is changed
SPM_WRITE:
	LDI		T1,CPU_CCP_SPM_GC		
	WR		CPU_CCP,T1
	WR		NVMCTRL_CTRLA,T2
RET

;##########################################################################################################################
#ifdef LPT
	ID_Start:	.db "Estl_Boot 1.01 LPT Adapter 00005"	ID_End:
#else
	ID_Start:	.db "Estl_Boot 1.01     Adapter 00006"	ID_End:
#endif

;##########################################################################################################################
.dseg
Buffer:		.byte 512	;Buffer to temporarily store 1 memory page
