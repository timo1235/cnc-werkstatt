;LR			LOAD		REGISTER WITH	IO / MEM BYTE
;LXW		LOAD  X		REGISTER WITH	WORD VALUE
;LZW		LOAD  Z		REGISTER WITH	WORD VALUE

;WR			WRITE		REGISTER TO		IO / MEM BYTE
;WV			WRITE BYTE	VALUE	 TO		IO / MEM BYTE
;WW			WRITE WORD	VALUE	 TO		IO / MEM WORD

;CCP_WRITE	WRITE BYTE VALUE TO A CONFIGURATION CHANGE PROTECTED REGISTER
;SPM_WRITE	WRITE BYTE VALUE TO THE PROTECTED NVM CONTROLLER REGISTER

;Wait_BIT_SET	WAIT FOR BIT IN IO TO SET
;Wait_BIT_CLR	WAIT FOR BIT IN IO TO CLEAR

.MACRO LR				
	.if @1 < 0x40
		IN	@0,@1
	.else
		LDS	@0,@1
	.endif
.ENDMACRO

.MACRO LXW
		LDI    XL, LOW(@0)
        LDI    XH, HIGH(@0)		         
.ENDMACRO

.MACRO LZW 
		LDI    ZL, LOW(@0)
        LDI    ZH, HIGH(@0)		         
.ENDMACRO

.MACRO WR
	.if @0 < 0x40
		OUT	@0,@1
	.else
		STS	@0,@1
	.endif
.ENDMACRO

.MACRO WV
		LDI T1,@1
		WR	@0,T1
.ENDMACRO

.MACRO WW
		WV @0,   LOW(@1)
		WV @0+1,HIGH(@1)
.ENDMACRO


.MACRO CCP_WRITE
	WR		CPU_CCP,R_CCP
	LDI		T1,@1					
	WR		@0,T1
.ENDMACRO

.MACRO Wait_BIT_SET
	WBS:	LR		T1,@0
			SBRS	T1,@1
			RJMP	 WBS
.ENDMACRO

.MACRO Wait_BIT_CLR
	WBC:	LR		T1,@0
			SBRC	T1,@1
			RJMP	WBC
.ENDMACRO