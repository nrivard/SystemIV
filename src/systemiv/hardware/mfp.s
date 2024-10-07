    section .text

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

mfp_init::
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
