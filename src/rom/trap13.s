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
    jsr     (A0)
.Done:
    rte

.FunctionTable:
    dc.l    SDCARDINIT                  ; call into direct
    dc.l    .SDCARD_RD_BLK
    dc.l    SPIINIT                     ; call into direct (should be removed!)
    dc.l    SPIASSERT                   ; call into direct (should be removed!)
    dc.l    SPIDEASSERT                 ; call into direct (should be removed!)
    dc.l    .SPI_TX                     ; should be removed!
.FunctionTableEnd:

; D1 should contain the byte to transfer
; on return, D0 will contain the return value
.SPI_TX:
    move.l  D1,D0
    jsr     SPITX
    rts

; D1 should contain the block number to read
.SDCARD_RD_BLK:
    move.l D1,D0
