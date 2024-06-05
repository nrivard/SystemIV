    section .text

; D0 contains function code
HANDLETRAP13::
    cmp.l   #(.FunctionTableEnd-.FunctionTable)/4,D0
    bge     .FunctionTableEnd
.Found:
    add.l   D0,D0
    add.l   D0,D0                       ; adding twice is faster than shifting or mul
    move.l  .FunctionTable(PC,D0),A0    ; get offset into function table
    jmp     (A0)

.FunctionTable:
    dc.l    SPIINIT
    dc.l    SPIASSERT
    dc.l    SPIDEASSERT
    dc.l    SPITX
    ; dc.l    SDCARDINIT
    ; dc.l    SDCARDREADBLK
.FunctionTableEnd:
    rte
