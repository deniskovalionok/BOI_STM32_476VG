	MODULE exception
	PUBLIC exception
	PUBLIC SVC_Handler
        PUBLIC interrupts_get_Registers
	extern swi_handler
        extern _reg_R13 _reg_R14 _reg_CPSR _reg_SPSR _reg_R0 _reg_R1 _reg_R2 _reg_R3 _reg_R4 _reg_R5 _reg_R6 _reg_R7 _reg_R8 _reg_R9 _reg_R10 _reg_R11 _reg_R12
        extern _stackVal1, _stackVal2, _stackVal3, _stackVal4;
	SECTION CODE:CODE:NOROOT(2) 
 
exception	// вызов обработчика исключений
      svc	0
 
      
SVC_Handler
      TST lr, #4 // Test for MSP or PSP
      ITE EQ
      MRSEQ r0, MSP
      MRSNE r0, PSP
      B swi_handler
      
      ;END
      
; Execution starts here.
; After a reset, the mode is ARM, Supervisor, interrupts disabled.

;get R14 register to use in C code
		
interrupts_get_Registers
		STR      LR,[SP, #-4]!
		MRS      LR,MSP
		STMDB    SP!,{R0-R2,LR}
		PUSH	{R12}

                
		LDR		R12, =_reg_R7
		STR		R7,[R12]
		LDR		R12, =_reg_R6
		STR		R6,[R12]
		LDR		R12, =_reg_R5
		STR		R5,[R12]
		LDR		R12, =_reg_R4
		STR		R4,[R12]
		LDR		R12, =_reg_R3
		STR		R3,[R12]
		LDR		R12, =_reg_R2
		STR		R2,[R12]
		LDR		R12, =_reg_R1
		STR		R1,[R12]
		LDR		R12, =_reg_R0
		STR		R0,[R12]
		POP		{R12}
		LDR		R0, =_reg_R12
		STR		R12,[R0]
		
		MRS      R2,PSP
		MRS      R1,MSP
		;prepare switch to mode from where exception was
		MOV      R0,R1
		AND      R0,R0,#0x1f
		ORR      R0,R0,#0xc0	;disable INT
		MSR      PSP,R0
                CPSID           I
		
		LDR		R0, =_reg_R11
		STR		R11,[R0]
		LDR		R0, =_reg_R10
		STR		R10,[R0]
		LDR		R0, =_reg_R9
		STR		R9,[R0]
		LDR		R0, =_reg_R8
		STR		R8,[R0]

		LDR		R0, =_reg_R13
		STR		R13,[R0]
		LDR		R0, =_reg_R14
		STR		R14,[R0]
		LDR		R0, =_reg_SPSR
		STR		R1,[R0]
		MOV		R0, R13
		LDR		R1,[R0,#-4]
		LDR		R0, =_stackVal1
		STR		R1,[R0]
		MOV		R0, R13
		LDR		R1,[R0,#-8]
		LDR		R0, =_stackVal2
		STR		R1,[R0]
		MOV		R0, R13
		LDR		R1,[R0,#-12]
		LDR		R0, =_stackVal3
		STR		R1,[R0]
		MOV		R0, R13
		LDR		R1,[R0,#-16]
		LDR		R0, =_stackVal4
		STR		R1,[R0]

		MSR      PSP,R2

		LDM      SP!,{R0-R2,LR}
		MSR      MSP,LR
		LDM      SP!,{PC}        ;; return



                END