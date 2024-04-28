    section .text

    public MFP_VECBASE, MFP_IMRB, MFP_ISRB, MFP_IRQ_TIMER_C, MFP_VEC_TIMER_C

MFP_BASE::                      equ     $E00001
MFP_GPIP::                      equ     MFP_BASE+($00<<1)
MFP_DDR::                       equ     MFP_BASE+($02<<1)
MFP_IERA                        equ     MFP_BASE+($03<<1)
MFP_IERB                        equ     MFP_BASE+($04<<1)
MFP_ISRA                        equ     MFP_BASE+($07<<1)
MFP_ISRB                        equ     MFP_BASE+($08<<1)
MFP_IMRA                        equ     MFP_BASE+($09<<1)
MFP_IMRB                        equ     MFP_BASE+($0A<<1)
MFP_VR                          equ     MFP_BASE+($0B<<1)
MFP_TCDCR                       equ     MFP_BASE+($0E<<1)
MFP_TCDR                        equ     MFP_BASE+($11<<1)
MFP_TDDR                        equ     MFP_BASE+($12<<1)
MFP_UCR                         equ     MFP_BASE+($14<<1)
MFP_RSR                         equ     MFP_BASE+($15<<1)
MFP_TSR                         equ     MFP_BASE+($16<<1)
MFP_UDR                         equ     MFP_BASE+($17<<1)

MFP_VR_IN_SERVICE_EN            equ     $08
MFP_TMR_DELAY_PRESCALE_4        equ     %001
MFP_TMR_DELAY_PRESCALE_200      equ     %111
MFP_UCR_DIVIDE_BY_16            equ     (%1<<7)
MFP_UCR_STOP_BIT_1              equ     (%01<<3)
MFP_RSR_BUFFER_FULL             equ     (%1<<7)
MFP_RSR_ENABLE                  equ     (%1)
MFP_TSR_BUFFER_EMPTY            equ     (%1<<7)
MFP_TSR_SET_HIGH                equ     (%1<<2)
MFP_TSR_ENABLE                  equ     (%1)

MFP_IRQ_TIMER_C::               equ     $20
MFP_IRQ_TIMER_C_MASK::          equ     ~MFP_IRQ_TIMER_C

MFP_VECBASE                     equ     $40
MFP_VEC_TIMER_C                 equ     ((MFP_VECBASE+$05)*4)   ; 5th vector, longword size

MFPINIT::
    move.b  #$FE,MFP_DDR                                        ; set all but bit 0 of GPIO as outputs
.Timer:                                                         ;         CLK     | prescale | UCR bit 7 | timer value | RC toggle
    move.b  #$B8,MFP_TCDR                                       ;  100 = (3686400 / 200      / n/a       / 184         / 1) IRQ on each transition!
    move.b  #$03,MFP_TDDR                                       ; 9600 = (3686400 / 4        / 16        / 3           / 2)
    move.b  #(MFP_TMR_DELAY_PRESCALE_200<<4|MFP_TMR_DELAY_PRESCALE_4),MFP_TCDCR
.UART:
    move.b  #(MFP_UCR_DIVIDE_BY_16|MFP_UCR_STOP_BIT_1),MFP_UCR  ; divide-by 16, async, 8N1
    move.b  #(MFP_TSR_SET_HIGH|MFP_TSR_ENABLE),MFP_TSR          ; set line default high, enable transmitter
    move.b  #(MFP_RSR_ENABLE),MFP_RSR                           ; enable receiver
.Interrupts:
    move.b  #(MFP_VECBASE|MFP_VR_IN_SERVICE_EN),MFP_VR          ; vector base at $40, enable software end-of-interrupt mode
    move.b  #(MFP_IRQ_TIMER_C),MFP_IERB                         ; enable interrupts for timer C (masked on reset)
    rts

; C-interface to test if a byte is available
mfp_byte_avail::
; Returns long value of zero if no byte avail, non-zero if byte is waiting
MFPBYTEAVAIL::
    move.b  MFP_RSR,D0
    andi.l  #$80,D0     ; mask all but buffer full bit
    rts

; C-interface to MFP routine
mfp_send::
    move.l  4(SP),D0
; Sends char in D0
MFPSEND::
    tst.b   MFP_TSR     ; ready to send?
    bpl     MFPSEND
.Send:
    move.b  D0,MFP_UDR
.Done:
    rts

mfp_receive::
; Receives char in D0
MFPRECV::
    clr.l   D0          ; clear contents of D0
    tst.b   MFP_RSR     ; ready to receive?
    bpl     MFPRECV
.Recv:
    move.b  MFP_UDR,D0
.Done:
    rts

; ; params:
; ;   D0: the long to convert to ascii and send
; MFPSendLong::
;     movem.l D0-D1,-(SP)
;     move.l  #3,D1
; .Loop:
;     rol.l   #8,D0
;     jsr     MFPSendByte
;     dbra    D1,.Loop
; .Done:
;     movem.l (SP)+,D0-D1
;     rts

; ; params:
; ;   D0: the byte to convert to ascii and send
; MFPSendByte::
;     move.l  D0,-(SP)        ; save D0
; .UpperNibble:
;     lsr.b   #4,D0           ; upper nibble
;     jsr NibbleToAscii
;     jsr MFPSend
; .LowerNibble:
;     move.l  (SP),D0        ; re-use D0
;     jsr NibbleToAscii
;     jsr MFPSend
; .Done:
;     move.l  (SP)+,D0        ; restore D0
;     rts

; NibbleToAscii::
;     and.b   #$0F,D0
;     ori.b   #'0',D0
;     cmpi.b  #('9'+1),D0     ; digit?
;     bcs     .Done
;     addq.b  #$07,D0         ; 'A' - '9' - 1
; .Done:
;     rts

; ; params:
; ;   A0: ptr to string
; ; returns:
; ;   D0: long value
; HexStringToLong::
;     movem.l D2-D3,-(SP)
;     pea.l   (A0)
;     jsr     strlen
;     move.l  (SP)+,A0
;     cmp.b   #8,D0               ; string greater than 8 digits?
;     bgt     .Done
;     move.b  D0,D2               ; move string index to D2 (reversing thru string)
;     subq.b  #1,D2               ; fix index
;     clr.l   D3                  ; clear our running result
;     clr.b   D1                  ; clear our shift
; .Loop:
;     move.b  (A0,D2),D0
;     and.l   #$FF,D0             ; clear upper bits
;     jsr     CharToByte
;     lsl.l   D1,D0
;     or.l    D0,D3
;     addq.b  #4,D1               ; increase the shift by 4
;     subq.b  #1,D2
;     bpl     .Loop
; .Done:
;     move.l  D3,D0
;     movem.l (SP)+,D2-D3
;     rts

; ; params:
; ;   D0: char to convert to ascii (only operates on byte)
; ; returns:
; ;   D0: ascii value of the byte (only updates byte)
; ;   NOTE: will return invalid ascii if char was not a number!
; CharToByte::
;     sub.b   #'0',D0     ; check 0...9
;     cmp.b   #$0A,D0
;     blt     .Done
;     sub.b   #$07,D0     ; 'A' - '9' - 1
;     cmp.b   #$10,D0     ; check A...F
;     blt     .Done
;     sub.b   #$20,D0     ; 'a' - 'A'
; .Done:
;     rts
