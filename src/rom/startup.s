    section text

    public Start

Vectors:
    dc.l    STACK_START                 ; $00: reset SP
    dc.l    Start                       ; $01: reset PC
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
VectorsEnd:
VectorCount: equ (VectorsEnd-Vectors)/4 ; 256 longwords

    dcb.l   8,HandleGenericInterrupt    ; $10-17: Reserved
    dc.l    HandleDebug                 ; $18: spurious interrupt
    dcb.l   7,HandleGenericInterrupt    ; $19-1F: autovectors
    
    dcb.l   13,HandleGenericInterrupt   ; $20-2C: unused Trap handlers
    dc.l    HandleGenericInterrupt      ; $2D: trap#13
    dc.l    HandleTrap14                ; $2E: trap#14
    dc.l    HandleGenericInterrupt      ; $2F: trap#15

    dcb.l   16,HandleGenericInterrupt   ; $30-3F reserved
    dcb.l   $C0,HandleGenericInterrupt  ; $40-FF user vectors

Start:
    lea.l   Vectors,A0                  ; copy vectors into RAM (overlay should have triggered)
    lea.l   $000000,A1
    move.w  #(VectorCount-1),D0 ; number of longwords to copy (should be $100...)
.VectorLoop:
    move.l  (A0)+,(A1)+                 ; copy contents pointed at by A0 into A1 and post-increment both pointers
    dbra    D0,.VectorLoop
    
HandleBusError:

HandleAddrError:

HandleIllegalInstr:

HandleGenericInterrupt:

HandleDebug:

HandleTrap14:
    
STACK_START: