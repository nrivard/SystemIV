    section text

    public ShellularMain

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
    jsr     ShellRunCommand
.Done:
    bra.s   .GetCommand

; params:
;   A0: pointer to CRLF terminated buffer of space separated tokens
;   A1: pointer to an array of 8 pointers (32 bytes)
; return:
;   D0: number of tokens found
;   A0: pointer to position _after_ CRLF
;   Note: This routine MODIFIES the buffer, replacing spaces with null-terminators!
;   A1: pointer to an array of pointers
;   Note: Use D0 to understand which indexes in table are valid pointers to tokens!
ShellTokenizeBuffer:
    move.l  D1,-(A7)            ; save D1
    clr.l   D0                  ; 0 tokens processed at start
.TokenLoop:
    move.l  A0,(A1,D0)          ; copy current value of A0 into ptr array indexed by D0
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
    move.l  (A7)+,D1            ; restore D1
    rts

ShellRunCommand:

; Params:
;   A0: null-terminated string to send
; return
;   A0: pointer to position _after_ null-termintor
SendString:
    move.l  D0,-(A7)            ; save D0
.Loop:
    move.b  (A0)+,D0
    beq     .Done               ; null terminator
    jsr     MFPSend
    bra.s   .Loop
.Done:
    move.l  (A7)+,D0            ; restore D0
    rts

    section bss

ShellCmdBuffer:     ds.b    $80     ; command buffer
ShellTokenTable:    ds.b    $20     ; table of pointers to tokens, up to 8 tokens
ShellSendTokens_DBG: ds.b   $00     ; if non-zero, echo parsed tokens

    section rodata

ASCII_CR        equ     $0D
ASCII_LF        equ     $0A

ShellVersion    equ     $01
ShellWelcome:   dc.b    "Shellular v", ShellVersion, "\r\n", $00
