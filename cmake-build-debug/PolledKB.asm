;
; Input and Output
;
; Device polling - example KB input and SCR output
; Echo each character read, stop on 'Z'
; R0 - base address (KbCSR address 0002)
; R1 - character read/written
; R2 - scratch register
;
; ECED 3403
; 21 Jun 2022 - XMX upgrade
; 25 Mar 2020 - XM3 upgrade
;  8 May 2019 - Initial version
;
; Device register structure:
;
;           7  6  5    4    3     2     1     0
; nnnn   | Reserved | ENA | OF | DBA | I/O | IE | Control/Status
; nnnn+1 |                Data                  | Data
;
; KB and SCR control bits (OF, DBA, and IE = 0):
;
KB_SET	equ	#12	; KB initialization: ENA=1 I/O=1
SCR_SET	equ	#10	; SCR initialization: ENA=1 I/O=0 
;
; KB and SCR status bits (check on poll):
;
KB_DBA	equ	#4	; KB - data byte available status bit (input)
SCR_DBA	equ	#4	; SCR - data buffer available status bit (output)
;
	org	#2
;
; Clock control/status and count registers are ignored
;
KbCSR	bss	#1	; Keyboard control/status register
KbDR	bss	#1	; Keyboard data register (input)
ScrCSR	bss	#1	; Screen control/status register
ScrDR	bss	#1	; Screen data register (output)
;
	org	#1000
Start
	movlz	KbCSR,A0	; A0 - address of KbSCR (base addr)
;
; Initialize KB and SCR
	movl	KB_SET,R2	; R2 = KB init bits
	st.b	R2,A0		; KB SCR = ENA and Input
	movl	SCR_SET,R2	; R2 = SCR init bits
	str.b	R2,A0,$2	; SCR CSR (A0 base addr + 2) = ENA and Output
;
; Input-output loop - repeat until 'Z' entered
;
; InLoop waits for character to be entered
; Read KB status register until DBA (data byte available) is set
;
InLoop	
	ld	A0,R2		; Check DBA (clear until data)
	and	KB_DBA,R2	; If zero, repeat
;
; Repeat KB status poll (DBA) if data byte not available
; 
	cex	eq,$1,$0
	br	InLoop		; Goto InLoop if DBA = 0
;
; Data byte available - read KbDR
;
	ldr.b	A0,#1,R1	; R1 = kb input (ch)
;
	str.b	R1,A0,#3	; ScrDR (KbCSR+3) = R1 (ch)
;
; Wait until output completed
; Poll output DBA (data buffer available)
;
OutLoop
	ldr.b	A0,#2,R2	; R2 = ScrCSR
	and.b	SCR_DBA,R2	; Is DBA set?
;
; Repeat SCR status poll (DBA) if data buffer not available
;
	cex	eq,$1,$0
	br	OutLoop		; Goto OutLoop if DBA = 0
;
; Output done
; Check for 'Z'
;
	movlz	'Z',R2
	cmp.b	R2,R1
;
; Repeat input-output if not 'Z'
;
	cex	ne,$1,$0
	br 	InLoop		; Goto InLoop if R2 <> 0
;
; All done - Wait for ^C or breakpoint
;
DoneLoop
	br	DoneLoop
;
	end	Start
