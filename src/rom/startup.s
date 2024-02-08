    section text

    public Start

Vectors:
    dc.l    STACK_START                 ; $00: start of stack
    dc.l    Start                       ; $01: ROM start
    dc.l    HandleBusError              ; $02: bus error handler
    dc.l    HandleAddrError             ; $03: address error handler
    dc.l    HandleIllegalInstr          ; $04: illegal instruction handler
    dc.l    HandleGenericInterrupt      ; $05: divide by zero
    dc.l    HandleGenericInterrupt      ; $06: CHK instr
    dc.l    HandleGenericInterrupt      ; $07: TRAPV instr
    dc.l    HandleGenericInterrupt      ; $08: privilege violation
    dc.l    HandleGenericInterrupt      ; $09: trace
    dc.l    HandleGenericInterrupt      ; $0A: line 1010 emulator
    dc.l    HandleGenericInterrupt      ; $0B: line 1111 emulator
    dcb.l   3,HandleGenericInterrupt    ; $0C-0E: Reserved
    dc.l    HandleGenericInterrupt      ; $0F: uninitialized vector

    dcb.l   8,HandleGenericInterrupt    ; $10-17: Reserved
    dcb.l   8,HandleGenericInterrupt    ; $18-1F: autovectors
    
    dcb.l   13,HandleGenericInterrupt   ; $20-2C: unused Trap handlers
    dc.l    HandleGenericInterrupt      ; $2D: trap#13
    dc.l    HandleTrap14                ; $2E: trap#14
    dc.l    HandleGenericInterrupt      ; $2F: trap#15

    dcb.l   16,HandleGenericInterrupt   ; reserved

Start:
    
HandleBusError:

HandleAddrError:

HandleIllegalInstr:

HandleGenericInterrupt:

HandleTrap14:
    
STACK_START:
