;
; Assignment 2 test program
; Test functionality of ADDA, SUBA, and CMPA instruction
;
org #1000
MAIN
; test ADDA instruction
	MOVLS #AB,R0	; R0 <- 0xFFAB
	MOVLZ #0,A0		; A0 <- 0x0000
ADDS	WORD #7140		; A0.lsb <- R0.lsb <- 0x00AB

	MOVLZ #0,A1		; A1 <- 0x0000
	MOVLS #0,A2		; A2 <- 0xFF00
	WORD #71b1		;	 A1 <- 0x0000 + 0x0020
	WORD #71aa		;	 A2 <- 0xFF00 + 0x0010

	MOVL #34,SP	
	MOVH #1200,SP	; SP <- 0x1234
	MOVL #21,BP
	MOVH #4300,BP	; BP <- 0x4321
	WORD #7334		; BP <- BP + SP = 0x5555


; SET BP for 0x1018
; A0 should be 0x00AB
; A1 should be 0x0020
; A2 should be 0xFF10
; BP should be 0x5555

	BL CLEARREG
	MOVLZ #00,LR

;test SUBA instruction
	MOVLS #88,R0	; R0 <- 0xFF88
	MOVLZ #97,A0	; A0 <- 0x0097
SUBS	WORD #7540		; A0.lsb <- A0.lsb - R0.lsb <- 0x000F

	MOVLZ #0,A1		; A1 <- 0x0000
	MOVLS #FF,A2	; A2 <- 0xFFFF
	WORD #7589		;	 A1 <- 0x0000 - 0x0001 = 0xFFFF
	WORD #75aa		;	 A2 <- 0xFFFF - 0x0010 = 0xFFEF

	MOVL #34,SP	
	MOVH #1200,SP	; SP <- 0x1234
	MOVL #21,BP
	MOVH #4300,BP	; BP <- 0x4321
	WORD #7734		; BP <- BP - SP = 0x30ED

; SET BP for 0x1034
; A0 should be 0x000F
; A1 should be 0xFFFF
; A2 should be 0xFFEF
; BP should be 0x30ED

	BL CLEARREG
	MOVLZ #00,LR

; CMPA instruction tests
	MOVLS #01,R0	; R0 <- 0xFF01
	MOVLZ #01,A0	; A0 <- 0x0001
CMPS	WORD #7940		; A0 == R0?

	CEX EQ,$1,$1	; should be true	
	MOVLZ $1,R1		; True condition, R1 <- 0x0001
	MOVLZ $1,R2		; False condition, R2 <- 0x0001

	MOVLZ #00,A1	; A1 <- 0x0000
	MOVLS #FF,A2	; A2 <- 0xFFFF
	WORD #790a		; A1 == A2?
	CEX EQ,$1,$1	; should be false
	MOVLZ $1,R3		; True condition, R3 <- 0x0001
	MOVLZ $1,R4		; False condition, R4 <- 0x0001

	MOVLZ #20,BP	; BP <- 0x0020
	WORD #79b4		;	 BP == 0x20?
	CEX EQ,$1,$1	; should be true
	MOVLZ $1,R5		; True condition, R5 <- 0x0001
	MOVLZ $1,R6		; False condition, R6 <- 0x0001

	MOVLS #10,LR	; LR <- 0xFF10
	WORD #79ad		;	 LR == 0x20?
	CEX EQ,$1,$1	; should be false
	MOVLZ $1,R7		; True condition, R7 <- 0x0001
	MOVLZ $-1,R7	; False condition, R7 <- 0x00FF

; SET BP for 0x1064
; R1 should be 0x0001
; R2 should be 0x0000
; R3 should be 0x0000
; R4 should be 0x0001
; R5 should be 0x0001
; R6 should be 0x0000
; R7 should be 0x00FF

DONE
	BR DONE ; busy wait loop

org #1100
CLEARREG
;Subroutine to clear all registers except PC and LR
	MOVLZ #00,R0
	MOVLZ #00,R1
	MOVLZ #00,R2
	MOVLZ #00,R3
	MOVLZ #00,R4
	MOVLZ #00,R5
	MOVLZ #00,R6
	MOVLZ #00,R7
	MOVLZ #00,A0
	MOVLZ #00,A1
	MOVLZ #00,A2
	MOVLZ #00,A3
	MOVLZ #00,BP
	MOVLZ #00,SP
	MOV LR,PC

END	MAIN


