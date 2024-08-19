    section .text

; D0 contains function code
; D0/A0 are "destroyed" (could contain return values)
HANDLETRAP13::
    cmp.l   #(.FunctionTableEnd-.FunctionTable)/4,D0
    bge     .Done
.Found:
    add.l   D0,D0
    add.l   D0,D0                       ; adding twice is faster than shifting or mul
    move.l  .FunctionTable(PC,D0),A0    ; get offset into function table
    jmp     (A0)
.Done:
    rte

.FunctionTable:
    dc.l    .SPI_INIT
    dc.l    .SPI_ASSERT
    dc.l    .SPI_DEASSERT
    dc.l    .SPI_TX
    ; dc.l    SDCARDINIT
    ; dc.l    SDCARDREADBLK
.FunctionTableEnd:

.SPI_INIT:
    jsr     SPIINIT
    rts

.SPI_ASSERT:
    jsr     SPIASSERT
    rts

.SPI_DEASSERT:
    jsr     SPIDEASSERT
    rts

; D1 should contain the byte to transfer
; on return, D0 will contain the return value
.SPI_TX:
    move.l  D1,D0
    jsr     SPITX
    rts
