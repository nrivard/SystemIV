    section .text

    public ShellularMain, SendString

ShellularMain:
    lea.l   ShellWelcome,A0
    jsr     SendString
.GetCommand:
    move.b  #'>',D0
    jsr     MFPSend
    lea.l   ShellCmdBuffer,A0
.WaitForInput:
    jsr     MFPReceive
    move.b  D0,(A0)+            ; store char in buffer
    jsr     MFPSend             ; echo character
    cmpi.b  #(ASCII_LF),D0      ; EOL?
    bne     .WaitForInput
.ProcessCommand:
    lea.l   ShellCmdBuffer,A0   ; point back to start of cmd buffer
    lea.l   ShellUserInput,A1
    jsr     ShellTokenizeBuffer
    jsr     ShellDebugTokens
    movea.l A1,A0
    jsr     ShellParseCommand
    tst.l   D0
    bne     .InvalidCommand
    lea.l   ShellUserInput,A0
    move.l  FUNC(A1),A2         ; userInput->function()
    jsr     (A2)
    tst.l   D0
    beq     .Done
.InvalidUsage:
    move.l  USAGE(A1),A0
    jsr     SendString
    bra.s   .Done
.InvalidCommand:
    lea.l   ShellInvalid,A0
    jsr     SendString
    lea.l   ShellUserInput,A0
    jsr     SendString
    lea.l   ShellInvalidEnd,A0
    jsr     SendString
.Done:
    lea.l   ShellEOL,A0
    jsr     SendString
    bra.w   .GetCommand

; params:
;   A0: pointer to CRLF terminated buffer of space separated tokens
;   A1: pointer to `UserInput` struct
; return:
;   A0: pointer to position _after_ CRLF
;       Note: This routine MODIFIES the buffer, replacing spaces with null-terminators!
;   A1: pointer to populated `UserInput` struct
;   Clobbers: D0, D1
ShellTokenizeBuffer:
    clr.l   D0                  ; 0 tokens processed at start
.TokenLoop:
    move.l  A0,PARAMS(A1,D0)    ; copy current value of A0 into ptr array indexed by D0
.CharLoop:
    move.b  (A0)+,D1            ; next char
    cmpi.b  #ASCII_CR,D1        ; end of string?
    beq     .TermToken
    cmpi.b  #' ',D1             ; end of token?
    bne     .CharLoop
.TermToken:
    move.b  #0,-1(A0)           ; null terminate token
    addq.l  #4,D0               ; indexing is not size-aware and each ptr is 4 bytes wide
.EOL:
    cmpi.b  #ASCII_LF,(A0)
    bne     .TokenLoop
.Done:
    lsr     #2,D0               ; convert to number of tokens, not how big tokens are
    move.b  D0,NUMTOKENS(A1)
    rts

; params:
;   A0: pointer to UserInput
; return:
;   D0: non-zero if valid Command found, 0 if not
;   A1: pointer to Command
ShellParseCommand:
    movem.l A2-A4,-(SP)
    move.l  (A0),A3             ; let's use A3 here
    lea.l   ShellCmdTable,A2    ; loop over the command table
    lea.l   ShellCmdTableEnd,A4 ; faster comparison
.Loop:
    pea.l   (A3)
    pea.l   CMD(A2)
    jsr     strcmp
    addq.l  #8,SP
    beq     .Done
    add.l   #12,A2              ; next command
    cmpa.l  A4,A2               ; end of table?
    bne     .Loop
.Done:
    move.l  A2,A1               ; move command pointer to A1
    movem.l (SP)+,A2-A4
    rts

; params:
;   A0: ptr to ShellUserInput
; returns:
;   D0: zero if successful, nonzero if error
ShellRead:
    movem.l D2/A2-A5,-(SP)
    move.l  A0,A5               ; move user input to A5
    clr.l   D2
    move.b  NUMTOKENS(A5),D2
    subq.b  #1,D2               ; ignore first param since it's command name
    bne     .ParseAddr          ; need at least a starting address
    moveq.l #1,D0               ; error! no addr param
    bra.s   .Done
.ParseAddr:
    move.l  4(A5),A0            ; address param
    move.l  A0,D0
    jsr     HexStringToLong     ; value in D0
    move.l  D0,A4               ; address to read from will be in A4

    ;;; DEBUG
    jsr     MFPSendLong
    move.b  #' ',D0
    jsr     MFPSend
    ;;; DEBUG

    cmp.b   #1,D2               ; is there length param?
    ble     .PrintMem           ; if not provided, just print 1 addr (already in D1)
.ParseLength:
    move.l  8(A5),A0            ; length param
    jsr HexStringToLong
    move.l  D0,D2               ; d1 will hold our length
.PrintMem:
    ;;; DEBUG
    move.l  D2,D0
    jsr     MFPSendByte
    lea.l   ShellEOL,A0
    jsr     SendString
    ;;; DEBUG

    clr.l   D1
.Loop:
    move.b  (A4,D1),D0          ; get value at addr
    jsr     MFPSendByte
    move.b  #' ',D0
    jsr     MFPSend
    addq.l  #1,D1               ; inc our index
    move.l  D1,D0
    and.l   #7,D0               ; newline?
    bne     .LoopCheck
    move.b  #ASCII_CR,D0
    jsr     MFPSend
    move.b  #ASCII_LF,D0
    jsr     MFPSend
.LoopCheck:
    cmp.l   D2,D1               ; index < length?
    blt     .Loop
    clr.l   D0                  ; success
.Done:
    movem.l (SP)+,D2/A2-A5
    rts

; params:
;   A0: ptr to ShellUserInput
; returns:
;   D0: zero if successful, nonzero if error
ShellWrite:
    movem.l D2/A2-A5,-(SP)
    move.l  A0,A5               ; move user input to A5
    clr.l   D2
    move.b  NUMTOKENS(A5),D2
    cmp.b   #3,D2
    bge     .ParseAddr          ; need at least a starting address and one byte
    moveq.l #1,D0               ; error! no addr param
    bra.s   .Done
.ParseAddr:
    move.l  4(A5),A0            ; address param
    move.l  A0,D0
    jsr     HexStringToLong     ; value in D0
    move.l  D0,A4               ; address to read from will be in A4

    ;;; DEBUG
    jsr     MFPSendLong
    move.b  #' ',D0
    jsr     MFPSend

    subq.l  #3,D2               ; number of bytes - 1 to be written for dbra
    move.l  8(A5),A3            ; pointer to first byte string to be written
.WriteLoop:
    move.l  A3,A0
    jsr     HexStringToLong
    clr.l   D0                  ; success
.Done:
    movem.l (SP)+,D2/A2-A5
    rts

ShellExecute:
ShellTransfer:

ShellDebugTokens:
    movem.l D0-D7/A0-A7,-(SP)   ; save all registers
    lea.l   ShellDebugMsg,A0
    jsr     SendString
.PrintUserInput:
    lea.l   ShellUserInput,A1
    move.l  #(ShellUserInputEnd-ShellUserInput-1),D1    ; number of bytes to print
.PrintUserInputLoop:
    move.b  D1,D2
    and.b   #3,D2
    bne     .PrintUserInputByte
    lea.l   ShellEOL,A0
    jsr     SendString
.PrintUserInputByte:
    move.b  (A1)+,D0
    jsr     MFPSendByte
    dbra    D1,.PrintUserInputLoop
.Done:
    lea.l   ShellEOL,A0
    jsr     SendString
    movem.l (SP)+,D0-D7/A0-A7   ; restore all registers
    rts

; Params:
;   A0: null-terminated string to send
; return
;   A0: pointer to position _after_ null-terminator
SendString:
    move.l  D0,-(SP)            ; save D0
.Loop:
    move.b  (A0)+,D0
    beq     .Done               ; null terminator
    jsr     MFPSend
    bra.s   .Loop
.Done:
    move.l  (SP)+,D0            ; restore D0
    rts

; params:
strcmp::
    move.l  8(sp),A1            ; str2
    move.l  4(sp),A0            ; str1
    clr.l   D0
    clr.l   D1
.Loop:
    move.b  (A0)+,D0            ; load str1 into D0
    beq.s   .EndStr1
    move.b  (A1)+,D1            ; load str2 into D0
    beq.s   .EndStr2
    sub.l   D1,D0               ; sub d1 from d0
    beq     .Loop
    bra.s   .Done
.EndStr1:
    tst.b   (A1)+
    beq     .Done
    move.l  #-1,D0              ; str2 is longer so "greater"
    bra     .Done
.EndStr2:
    move.l  #1,D0               ; can only get here if str1 wasn't zero but str2 is, so str1 is "greater"
.Done:
    rts
    
strlen::
    move.l  4(SP),A0
.Loop:
    tst.b   (A0)+
    bne     .Loop
.Done
    sub.l   4(SP),A0
    subq.l  #1,A0
    move.l  A0,D0
    rts

    section .bss

ShellCmdBuffer::        ds.b    $80     ; command buffer
ShellSendTokens_DBG:    ds.w    $01     ; if non-zero, echo parsed tokens

PARAMS          equ $00         ; `UserInput` offset for start of params
NUMTOKENS       equ $28         ; `UserInput` offset for the number of tokens

ShellUserInput:
    ds.l    10              ; pointers for up to 10 params: <cmd> <addr> (<option1>...<option 8>)
    ds.b    1               ; number of params encountered
ShellUserInputEnd:

USER_INPUT_SIZE equ     (ShellUserInputEnd-ShellUserInput)

    section .rodata

ASCII_CR        equ     $0D
ASCII_LF        equ     $0A

; offsets into the `ShellCmdTable` "struct"
CMD             equ     $00
FUNC            equ     $04
USAGE           equ     $08

    ; typedef struct {
    ;    char *name;
    ;    void (*function)(void);
    ; } Command;
    macro Command
        dc.b    \1
        dc.l    \2
        dc.l    \3
    endm

ShellCmdTable:  
    Command "rd\0\0", ShellRead, ShellReadUsg
    Command "wr\0\0", ShellWrite, ShellWriteUsg
    Command "tx\0\0", ShellTransfer, ShellTxUsg
    Command "ex\0\0", ShellExecute, ShellExUsg
ShellCmdTableEnd:

ShellEOL::       dc.b   "\r\n\0"

ShellReadUsg:   dc.b    "rd <addr> (<length>)\0"
ShellWriteUsg:  dc.b    "wr <addr> <value1> (<value2>...<value8>)\0"
ShellTxUsg:     dc.b    "tx <addr>\0"
ShellExUsg:     dc.b    "ex <addr>\0"

ShellVersion    equ     ($01+'0')
ShellWelcome:   dc.b    "Shellular v", ShellVersion, "\r\n\0"
ShellInvalid:   dc.b    "Invalid command: \"\0"
ShellInvalidEnd: dc.b   "\"\0"
ShellBadUsg:    dc.b    "Usage:\0"
ShellDebugMsg:  dc.b    "DEBUGGING TOKENS\r\n\0"
