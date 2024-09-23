    section .text

; D1 contains function code
HANDLETRAP13::
    move.l  D1,-(SP)                    ; order matters here because calls below will pull
    move.l  A1,-(SP)                    ; from the stack when A1/D1 was originally used!
    cmp.l   #(.FunctionTableEnd-.FunctionTable)/4,D1
    bge     .Done
.Found:
    add.l   D1,D1
    add.l   D1,D1                       ; adding twice is faster than shifting or mul
    move.l  .FunctionTable(PC,D0),A1    ; get offset into function table
    jsr     (A1)
.Done:
    move.l  (SP)+,A1
    move.l  (SP)+,D1
    rte

.FunctionTable:
    dc.l    .SDCARD_INIT
    dc.l    .SDCARD_RD_BLK
    dc.l    .XMODEMRECV
    dc.l    SPIINIT                     ; call into direct (should be removed!)
    dc.l    SPIASSERT                   ; call into direct (should be removed!)
    dc.l    SPIDEASSERT                 ; call into direct (should be removed!)
    dc.l    .SPI_TX                     ; should be removed!
.FunctionTableEnd:

; D0: byte to transfer
.SPI_TX:
    jsr     SPITX
    rts

; A0: pointer to `sdcard_device_t` sized region
.SDCARD_INIT:
    move.l  A0,-(SP)
    jsr     sdcard_init
    addq.l  #4,SP
    rts

; D0: block number to read
; A0: pointer to allocated 512 byte buffer
; A1: pointer to allocated 1 byte buffer (no longer in A1 but originally should be. get off stack instead)
.SDCARD_RD_BLK:
    move.l  4(SP),-(SP)                 ; copy original A1 onto the stack
    move.l  A0,-(SP)
    move.l  D0,-(SP)
    jsr     sdcard_read_block
    add.l   #12,SP
    rts

; D0: maxsize of the destination buffer
; A0: pointer to buffer that is at least `maxsize` size
.XMODEMRECV:
    move.l  D0,-(SP)
    move.l  A0,-(SP)
    jsr     xmodem_recv
    add.l   #8,SP
    rts
