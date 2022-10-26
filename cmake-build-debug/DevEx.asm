;
; Device example:
; Enabling and polling
; ECED 3403
; 
;
; Device register structure:
;
;           7  6  5    4    3     2     1     0
; nnnn   | Reserved | ENA | OF | DBA | I/O | IE | Control/Status
; nnnn+1 |                Data                  | Data
;
;
; CLK, KB, and SCR control bits (ENA and IE = 0):
; Set or clear to initialize device
; I/O bit is assigned by device
;
CLKdis	equ	#0	; CLK disable
CLKena	equ	#10	; CLK enable
KBinit	equ	#12	; KB enable: ENA=1
SCRinit	equ	#10	; SCR enable: ENA=1
;
; KB and SCR status bits (check on poll using BIT):
;
CLKdba	equ	#2	; CLK - time limit reached
KBdba	equ	#2	; KB - data byte available status bit (input)
SCRdba	equ	#2	; SCR - data buffer available status bit (output)
;
	org	#0
;
; Device control/status and count registers 
;
ClkCSR	bss	#1	; Clock control/status register
ClkDR	bss	#1	; Clock data register (write)
KbCSR	bss	#1	; Keyboard control/status register
KbDR	bss	#1	; Keyboard data register (read)
ScrCSR	bss	#1	; Screen control/status register
ScrDR	bss	#1	; Screen data register (write)
;
; **********************************************
;
; Device programming examples
; Set PC to starting address
;
; **********************************************
; 
; Keyboard example
; Set PC to #1000
;
	org	#1000
KBStart
;
; Initialize device (Enable and Input)
; Poll until data available 
;
	movlz	KbCSR,A0	; A0 <- addr KbCSR (0002)
;
; Initialize device
;
	movlz	KBinit,R0	; R0 <- KBinit (ENA=1, I/O=1)
	st.b	R0,A0		; [KbCSR] <- KBinit
;
; Poll for input
; Count number of times through loop
;
	movlz	#0,R2		; R2 <- 0 (poll count)
;
; DBA = 0: KB does not have data
; DBA = 1: KB has data
;
KBPoll
	ld.b	A0,R0		; R0 <- [KbCSR]
	bit	KBdba,R0	; KB DBA set?
	cex	EQ,$2,$0	; Equal to zero?
	add	#1,R2		; 0: Incr poll count
	br	KBPoll		; 0: repeat poll
;
; KB data exists, read KBDR (KbCSR + 1)
;
	ldr.b	A0,$1,R1	; R1 <- [KbCSR]
;
KBDone
	br	KBDone		; Put Breakpoint here, Check R1, R2
;
; **********************************************
;
; Screen example
; Set PC to #2000
;
	org	#2000
SCRStart
;
; Initialize device (Enable and Output)
; Poll until buffer available (write done)
;
	movlz	ScrCSR,A0	; A0 <- addr ScrCSR (0004)
;
; Initialize device
;
	movlz	SCRinit,R0	; R0 <- SCRinp (ENA=1, I/O=0)
	st.b	R0,A0		; [ScrCSR] <- SCRinit
;
; Write data to screen
; SCRdata <- character
;
	movlz	'X',R1		; R1 <- 'X'
	str.b	R1,A0,$1	; [A0 + 1] <- R0
;
; Poll for output done (buffer available)
; Count number of times through loop
;
	movlz	#0,R2		; R2 <- 0 (poll count)
;
; DBA = 0: SCR busy
; DBA = 1: SCR ready
;
SCRPoll
	ld.b	A0,R0		; R0 <- [ScrCSR]
	bit	SCRdba,R0	; SCR DBA set?
	cex	EQ,$2,$0	; Equal to zero?
	add	#1,R2		; 0: Incr poll count
	br	SCRPoll		; 0: repeat poll
;
; Write data to screen
; SCRdata <- character
;
	movlz	'Y',R1		; R1 <- 'Y'
	str.b	R1,A0,$1	; [A0 + 1] <- R0
;
; 'Y' should appear here if no breakpoint (^C to exit)
SCRDone
	br	SCRDone		; ^C here, check R2
;
;
; **********************************************
;
; Clock (timer) example
;
; CLKdata is different from KB or SCR:
; Input device (when clock expires)
; Write to data register to specify millisecond clock divisor
;
; CLKdata indicates ticks per second (approx)
;
; CLKdata = 1 (or 0) -  1 'tick' = ~1 second
; CLKdata = FF (255) - 255 'ticks' = ~3.9 milliseconds
;
; Variable clock limit (1 to 255)
; # of clock tick per second
;
	org	#20
TPS	byte	#1	; Default 1 second
;
; Set PC to #3000
;
	org	#3000
CLKStart
;
; Initialize device (Enable and Input)
; Poll until buffer available (time expired)
;
	movlz	ClkCSR,A0	; A0 <- addr ClkCSR (0006)
;
; Initialize device
; Three step process:
; 1. Disable clock
; 2. Initialize divisor
; 3. Enable device
;
; 1. Disable CLK
;
	movlz	CLKdis,R0	; R0 <- CLKdis 
	st.b	R0,A0		; [ClkCSR] <- CLKdis
;
; 2. Initialize
;
; Set ClkDR for #ticks/sec
;
	movlz	TPS,A1
	ld.b	A1,R0		; R0 <- Ticks per second
	str.b	R0,A0,$1	; [ClkDR] <- R0 (TPS)
;
; 3. Enable device
;
	movlz	CLKena,R0	; R0 <- CLKena (ENA=1)
	st.b	R0,A0		; [ClkCSR] <- CLKena
;
; Write data to screen (start of timing)
; Enable screen device
; SCRdata <- character
;
	movlz	ScrCSR,A1	; A1 <- addr of ScrCSR
	movlz	SCRinit,R1	; R1 <- screen ENA
	st.b	R1,A1
	movlz	'A',R1		; R1 <- 'A'
	str.b	R1,A1,$1	; [A1+1] <- R0
;
; Poll for output done (buffer available)
; Count number of times through loop
;
	movlz	#0,R2		; R2 <- 0 (poll count)
;
; DBA = 0: Clock < stop time
; DBA = 1: Clock >= stop time
;
CLKPoll
	ld.b	A0,R0		; R0 <- [ClkCSR]
	bit	CLKdba,R0	; CLK DBA set?
	cex	EQ,$2,$0	; Equal to zero?
	add	#1,R2		; 0: Incr poll count
	br	CLKPoll		; 0: repeat poll
;
; Write data to screen (end of timing)
; SCRdata <- character
;
	movlz	'Z',R1		; R1 <- 'Z'
	str.b	R1,A1,$1	; [A1+1] <- R0
CLKDone
	br	CLKDone		; Put Breakpoint here, check R2
;
	end

