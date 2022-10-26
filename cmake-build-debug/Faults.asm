;
; Fault and trap tests
; Test XMX faults and trap
;
; Ask user for fault test 
; 1 - ill inst; 2 - ill addr; 3 - priority; default - trap
;
; Samples:
;
; Illegal instruction
;	word	#7FFF	; HCF
;
; Illegal address
;	movl	#1,PC	; PC <- odd address
;
; Priority fault
;	setpri	#2	; Lower it
;	setpri	#4	; Attempt to raise it
;
; Double fault (FB):
; First, cause a fault using any of the above
; Second, cause a second fault in the exception handler
;
; Larry Hughes
; 5 Jul 22 - XMX revisions and cosmetic changes
; 1 Jul 21 - First version
;
; ************************************************************************
;
; Equates
;
NUL	equ	#0	; NUL char
SCR_SET	equ	#10	; SCR initialization: ENA=1, I/O=0, IE=0
SCR_DBA	equ	#4	; SCR - data buffer available status bit (output)
KB_SET	equ	#12	; KB initialization: ENA=1, I/O=1, IE=0
KB_DBA	equ	#4	; KB - data byte available status bit (input)
;
; System faults vector addresses
; Word pairs: PSW and Handler Entry Point
;
ILL_INST	equ	#FFE0	; Illegal instruction vector (8)
ILL_ADDR	equ	#FFE4	; Illegal PC address vector (9)
PRI_FLT		equ	#FFE8	; Priority fault vector (10)
DBL_FLT		equ	#FFEC	; Double fault (11)
OS_ENTRY	equ	#FFF0	; O/S entry point
ILL_VECT13	equ	#FFF4	; Not used
ILL_VECT14	equ	#FFF8	; Not used
RESET_VECT	equ	#FFFC	; Reset vector
;
STK_TOP		equ	#FF00
;
; ************************************************************************
; Device memory
; ************************************************************************
; 
	org	#0
ClkCSR	bss	$1	; Clock
ClkDR	bss	$1
KbCSR	bss	$1	; Keyboard
KbDR	bss	$1
ScrCSR	bss	$1	; Screen
ScrDR	bss	$1
;
; ************************************************************************
; Interrupt vector table
; ************************************************************************
;
; Exception addresses
;
; Uses vector 12 (clock ISR) for SVC since SVC can trap to any of
; the 16 vectors (15 plus reset).
; Vector contains new PSW (word 0) and new PC (address of ISR or
; exception ISR in this case).
;
; Vectors 0 to 7 (including device vectors) omitted
;
; Vector 8 - Illegal Instruction
	org	ILL_INST	; 8
	word	#00E0
	word	Ill_Inst
; Vector 9 - Illegal Address
	org	ILL_ADDR	; 9
	word	#00E0
	word	Ill_Addr
; Vector 10 - Priority Fault
	org	PRI_FLT		; 10
	word	#00E0		; Priority 7 - highest
	word	PriFault
; Vector 11 - Double Fault
	org	DBL_FLT		; 11
	word	#00E0		; Priority 7 - highest
	word	DblFault
; Vector 12 - SVC
	org	OS_ENTRY
	word	#00C0		; Priority 6
	word	OS_EP
;
; Reset vector
;
	org	RESET_VECT
	word	#4060	; Prev: 2, Curr: 3
	word	Reset	; Reset address the program
;
; ************************************************************************
; Code
; ************************************************************************
;
	org	#600
;
; Reset entry point
; Fault reset; PrevPri = 2; CurrPri = 3
; Should reset machine
;
Reset	mov	R0,R0		; Busy wait (for ^C)
	br	Reset
;
; ************************************************************************
; Mainline
; ************************************************************************
;
	org	#1000
;
; Get input, select fault (1, 2, 3).
; None of these, default trap
;
Start
;
; Create stack
;
	movl	STK_TOP,SP
	movh	STK_TOP,SP
;
; Useful marker
;
	movls	$0,LR	; Show on stack as FF00
;
; Change priority to 2 (startup is 7)
;
	setpri	#2
;
; Wait for KB input to select action
; 1 (inst), 2 (addr), 3 (pri), otherwise Trap
;
	movlz	KbCSR,A0
	movlz	KB_SET,R0
	st.b	R0,A0		; KB enabled
;
; Wait for keystroke
;
KbWait
	ld	A0,R0		; KbCSR (A0) -> R0
	and	KB_DBA,R0	; DBA set?
	cex	eq,#1,#0	; =0?
	br	KbWait		; Yes, poll again
;
; Read character
;
	ldr.b	A0,#1,R1	; R1 = ch entered
;
; Determine fault
;
	movlz	'1',R0
	cmp.b	R0,R1		; R1 = '1'?
	cex	eq,#1,#0
;
	word	#7FFF		; Yes: Illegal Instruction test
;
	movlz	'2',R0
	cmp.b	R0,R1
	cex	eq,#1,#0	; ='2'?
;
	movlz	#1,PC		; Yes: Illegal Address test
;
	movlz	'3',R0
	cmp.b	R0,R1
	cex	eq,#1,#0	; ='3'?
;
	setpri	#5		; Yes: Priority fault (currently 2)
;
; None of the above, do a trap
;
; SVC - Call to O/S
;
	movlz	#FF,R0		; R0 = 00FF 
	svc	$12		; Trap through vector 12
;
; Return here
; Check R0, should be FFAA
;
TrapStop
	setcc	s		; Put CPU to sleep - wait for ^C
	br	TrapStop
;
; ************************************************************************
; Handler code in high memory
; ************************************************************************
;
	org	#ff00
;
; SVC $12 to Trap Entry 
; For example, entry to Operating System
;
OS_EP
	movls	#AA,R0	; R0 = FFAA to indicate SVC successful
	mov	LR,PC
;
; ************************************************************************
;
; Fault handlers
; Display message and stop processing
;
; ILL_INST (8) - illegal instruction
; 
Ill_Inst
	movls	Ill_msg,A2
	br	FltMsg_Disp
;
; ILL_ADDR (9) - illegal address (PC on odd boundary)
;
Ill_Addr
	movls	Ill_adr,A2
	br	FltMsg_Disp
;
; PRI_FLT (10) - Priority fault - priority X to a priority < X
;
PriFault
	movls	Pri_flt,A2
	br	FltMsg_Disp
;
; DBL_FLT (11) - Double fault
;
DblFault
	movls	Dbl_flt,A2
	br	FltMsg_Disp
;
; ************************************************************************
;
; Fault display message
; Polled output, not interrupt driven (in case fault in ISR code)
; A2 contains address of message
;
FltMsg_Disp
;
	movlz	ScrCSR,A0	; A0 = addr ScrCSR
	movl	SCR_SET,R1	; R1 = SCR_SET
	st.b	R1,A0		; [ScrCSR] = SCR_SET
Flt_Disp0
	ld.b	A2+,R3		; R3 = [R2]; R2++
	cmp.b	NUL,R3		; R3 = 0 ?
	cex	eq,$1,$0	; eq?
	br	Flt_Disp2	; Yes: Goto Flt_Disp2 
Flt_Disp1
	ld.b	A0,R1		; R1 = [ScrCSR]
	and.b	SCR_DBA,R1	; R1.DBA = 1?
	cex	eq,$1,$0	; No (=0)
	br	Flt_Disp1
; Output
	str.b	R3,A0,$1	; [ScrDR] = R2
	br	Flt_Disp0
;
Flt_Disp2
	setcc	s		; put CPU to sleep
	br	Flt_Disp2
;
Ill_msg	ascii	"Illegal message\0"
Ill_adr	ascii	"Illegal address\0"
Pri_flt	ascii	"Priority fault\0"
Dbl_flt	ascii	"Double fault\0"
;
	end	Start
