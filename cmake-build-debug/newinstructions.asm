;Assembly code to test new instructions
;
;Put interrupts into table
org #FFC2
	WORD #00E0
	WORD #6660
;
;Interrupts called
org #6660
	MOVLZ #01,A1
	MOV LR,PC
;
;Main code
org #1000
;Put space on stack
	MOVL #ff,SP
;
	SETPRI #2
	MOVLZ #01,A0
	SVC #1
	MOVLZ #01,A2
;
;Set breakpoint for 100A
;A0, A1, and A2 should be 1
;
    SETCC CZNV
;Set breakpoint for 100C to see all PSW bits except for SLP set
    CLRCC CZNV
;Set breakpoint for 100E to see all PSW bits except for SLP cleared

	Done BR Done

end #1000