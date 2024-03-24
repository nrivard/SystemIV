    section text

    public MFPInit, MFPSend, MFPReceive, MFP_GPIP

MFP_BASE    equ $E00001
MFP_GPIP    equ MFP_BASE+($00<<1)
MFP_DDR     equ MFP_BASE+($02<<1)
MFP_TCDCR   equ MFP_BASE+($0E<<1)
MFP_TCDR    equ MFP_BASE+($11<<1)
MFP_TDDR    equ MFP_BASE+($12<<1)
MFP_UCR     equ MFP_BASE+($14<<1)
MFP_RSR     equ MFP_BASE+($15<<1)
MFP_TSR     equ MFP_BASE+($16<<1)
MFP_UDR     equ MFP_BASE+($17<<1)

MFP_TIMER_DELAY_PRESCALE_4      equ     %001
MFP_UCR_DIVIDE_BY_16            equ     (%1<<7)
MFP_UCR_STOP_BIT_1              equ     (%01<<3)
MFP_RSR_BUFFER_FULL             equ     (%1<<7)
MFP_RSR_ENABLE                  equ     (%1)
MFP_TSR_BUFFER_EMPTY            equ     (%1<<7)
MFP_TSR_SET_HIGH                equ     (%1<<2)
MFP_TSR_ENABLE                  equ     (%1)

MFPInit:
    move.b  #$FE,MFP_DDR                                        ; set all but bit 0 of GPIO as outputs
.Timer:
    move.b  #$03,MFP_TDDR                                       ;         CLK     | prescale | UCR bit 7 | timer value | RC toggle
    move.b  #(MFP_TIMER_DELAY_PRESCALE_4),MFP_TCDCR             ; 9600 = (3686400 / 4        / 16        / 3           / 2)
.UART:
    move.b  #(MFP_UCR_DIVIDE_BY_16|MFP_UCR_STOP_BIT_1),MFP_UCR  ; divide-by 16, async, 8N1
    move.b  #(MFP_TSR_SET_HIGH|MFP_TSR_ENABLE),MFP_TSR          ; set line default high, enable transmitter
    move.b  #(MFP_RSR_ENABLE),MFP_RSR                           ; enable receiver
    rts

; Sends char in D0
MFPSend:
    tst.b   MFP_TSR     ; ready to send?
    bpl     MFPSend
.Send:
    move.b  D0,MFP_UDR
.Done:
    rts

; Receives char in D0
MFPReceive:
    tst.b   MFP_RSR     ; ready to receive?
    bpl     MFPReceive
.Recv:
    move.b  MFP_UDR,D0
.Done:
    rts

MFPSendByte:
    