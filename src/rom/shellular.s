    section .text

    public ShellularMain, SendString

ShellularMain:
    jsr     MFPReceive          ; flush line
.SendGreeting:
    lea.l   ShellWelcome,A0
    jsr     SendString
.GetCommand:
    move.b  #'>',D0
    jsr     MFPSend
    lea.l   ShellCmdBuffer,A0
.WaitForInput:
    jsr     MFPReceive
    jsr     MFPSend             ; echo character
    move.b  D0,(A0)+            ; store char in buffer
    cmpi.b  #(ASCII_LF),D0      ; EOL?
    bne     .WaitForInput
.ProcessCommand:
    jsr     ShellTokenizeBuffer
    movea.l A1,A0               ; move pointer to tokenized buffer to A0
    jsr     ShellParseCommand
.Done:
    bra.s   .GetCommand

; params:
;   A0: pointer to CRLF terminated buffer of space separated tokens
;   A1: pointer to `UserInput` struct
; return:
;   A0: pointer to position _after_ CRLF
;   Note: This routine MODIFIES the buffer, replacing spaces with null-terminators!
;   A1: pointer to populated `UserInput` struct
ShellTokenizeBuffer:
    movem.l D0-D1,-(SP)            ; save D1
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
    movem.l (SP)+,D1-D0            ; restore D1
    rts

; params:
;   D0: number of tokens (though this function only checks that it's greater than zero!)
;   A0: pointer to tokenized buffer
ShellParseCommand:
    lea.l   ShellCmdTable,A1    ; loop over the command table
    clr.l   D1                  ; D1 will be our index into the commaand table


ShellRead:
ShellWrite:
ShellExecute:
ShellTransfer:


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

    section .bss

ShellCmdBuffer:         ds.b    $80     ; command buffer
ShellSendTokens_DBG:    ds.w    $01     ; if non-zero, echo parsed tokens

PARAMS          equ $00         ; `UserInput` offset for start of params
NUMTOKENS       equ $28         ; `UserInput` offset for the number of tokens

    macro UserInput
        ds.l    10              ; pointers for up to 10 params: <cmd> <addr> (<option1>...<option 8>)
        ds.b    1               ; number of params encountered
    endm

ShellUserInput: UserInput

    section .rodata

ASCII_CR        equ     $0D
ASCII_LF        equ     $0A

; offsets into the `ShellCmdTable` "struct"
CMD             equ     $00
FUNC            equ     $02

    ; typedef struct {
    ;    char name[2];
    ;    void (*function)(void);
    ; } Command;
    macro Command
        dc.b    \1
        dc.l    \2
    endm


ShellCmdTable:  
    Command "rd", ShellRead
    Command "wr", ShellWrite
    Command "tx", ShellTransfer
    Command "ex", ShellExecute
ShellCmdTableEnd:

ShellVersion    equ     $01
ShellWelcome:   dc.b    "Shellular v", ShellVersion, "\r\n", $00
