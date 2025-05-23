SystemClockHz   equ     100

    section .vectors

Vectors::
    dc.l    $100000                     ; $00: reset SP
    dc.l    START                       ; $01: reset PC
    dc.l    HandleBusError              ; $02: bus error handler
    dc.l    HandleAddrError             ; $03: misaligned memory access
    dc.l    HandleIllegalInstr          ; $04: illegal instruction handler
    dc.l    HandleGenericInterrupt      ; $05: divide by zero
    dc.l    HandleGenericInterrupt      ; $06: CHK instr resulted in out of bounds
    dc.l    HandleGenericInterrupt      ; $07: TRAPV instr
    dc.l    HandleGenericInterrupt      ; $08: privilege violation
    dc.l    HandleGenericInterrupt      ; $09: trace mode
    dc.l    HandleGenericInterrupt      ; $0A: 1010 (line $A) emulator
    dc.l    HandleGenericInterrupt      ; $0B: 1111 (line $F) emulator
    dcb.l   3,HandleGenericInterrupt    ; $0C-0E: Reserved
    dc.l    HandleGenericInterrupt      ; $0F: uninitialized interrupt vector

    dcb.l   8,HandleGenericInterrupt    ; $10-17: Reserved
    dc.l    HandleDebug                 ; $18: spurious interrupt
    dcb.l   7,HandleGenericInterrupt    ; $19-1F: autovectors
    
    dcb.l   13,HandleGenericInterrupt   ; $20-2C: unused Trap handlers
    dc.l    HANDLETRAP13                ; $2D: trap#13
    dc.l    HandleTrap14                ; $2E: trap#14
    dc.l    HandleGenericInterrupt      ; $2F: trap#15

    dcb.l   16,HandleGenericInterrupt   ; $30-3F reserved
    dcb.l   $C0,HandleGenericInterrupt  ; $40-FF user vectors
VectorsEnd:
    
VectorCount: equ (VectorsEnd-Vectors)/4 ; 256 longwords

    section .text

START::
    or.w    #$0700,SR                   ; Disable interrupts
.CopyVectors:
    lea.l   Vectors,A0                  ; copy vectors into RAM (overlay should have triggered)
    lea.l   VECT_START,A1
    move.l  #(VectorCount-1),D0         ; number of longwords to copy (should be $100...)
.VectorLoop:
    move.l  (A0)+,(A1)+                 ; copy contents pointed at by A0 into A1 and post-increment both pointers
    dbra    D0,.VectorLoop
.InitSystem:
    bsr.w   SYSINIT
    jsr     MFPINIT
    jsr     SPIINIT
.Heartbeat:
    lea.l   HandleTick,A0               ; copy HandleTick to timer c vector
    lea.l   MFP_VEC_TIMER_C,A1
    move.l  A0,(A1)
    bset.b  #5,MFP_IMRB                 ; enable timer c interrupts
    and.w   #$F8FF,SR                   ; enable all interrupts
.Done:
    jsr     sysmain
    move.w  SR,D0

SYSINIT:
    clr.b   SystemJiffies
    clr.l   SystemUptime
    rts

HandleBusError:
    move.l  #IRQBusErr,-(SP)
    jsr     serial_put_string
    bra.s   PrintExceptionInfo
HandleAddrError:
    move.l  #IRQAddrErr,-(SP)
    jsr     serial_put_string
PrintExceptionInfo:
    addq.l  #4,SP                       ; pop stack
;     move.l  #13,D1                      ; how many bytes we're sending
; .Loop:
;     btst    #0,D1
;     beq     .Print
;     lea.l   ShellEOL,A0
;     jsr     SendString
; .Print:
;     move.b  (SP)+,D0
;     jsr     MFPSendByte
;     dbra    D1,.Loop
.Done:
    bra.s   .Done                       ; we're cooked so start loopin'

HandleIllegalInstr:
HandleGenericInterrupt:
HandleDebug:
HandleTrap14:
    move.l  IRQError,-(SP)
    jsr     serial_put_string
    addq.l  #4,SP
    rte

HandleTick:
    move.l  D0,-(SP)
    move.b  SystemJiffies,D0
    addq.b  #1,D0
    cmp.b   #SystemClockHz,D0
    bne     .Done
.ResetJiffies:
    clr.b   D0
    addq.l  #1,SystemUptime
.Done:
    move.b  D0,SystemJiffies                ; write the jiffies value
    move.b  #MFP_IRQ_TIMER_C_MASK,MFP_ISRB  ; ack the interrupt
    move.l  (SP)+,D0
    rte

    section .bss

SystemJiffies:: ds.b    $01
SystemUptime::  ds.l    $01

    section .rodata

IRQError:   dc.b    "IRQ Error!\n\0"
IRQBusErr:  dc.b    "Bus Error!\n\0"
IRQAddrErr: dc.b    "Addr Error!\n\0"
