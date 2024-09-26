    section .text

; D1 values for the various trap routines available
TRAP13_SDCARD_INIT::        equ     0
TRAP13_SDCARD_RD_BLK::      equ     1
TRAP13_SDCARD_RD_BLK_N::    equ     2
TRAP13_SDCARD_WR_BLK::      equ     3
TRAP13_SDCARD_WR_BLK_N::    equ     4
TRAP13_XMODEM_RECV::        equ     5
TRAP13_SERIAL_GET::         equ     6
TRAP13_SERIAL_PUT::         equ     7
TRAP13_SERIAL_BYTEAVAIL::   equ     8

; D1 contains function code
HANDLETRAP13::
    move.l  D1,-(SP)                    ; order matters here because calls below will pull
    move.l  A1,-(SP)                    ; from the stack when A1/D1 are used!
    cmp.l   #(.FunctionTableEnd-.FunctionTable)/4,D1
    bge     .Done
.Found:
    add.l   D1,D1
    add.l   D1,D1                       ; adding twice is faster than shifting or mul
    move.l  .FunctionTable(PC,D1),A1    ; get offset into function table
    jsr     (A1)
.Done:
    move.l  (SP)+,A1
    move.l  (SP)+,D1
    rte

.FunctionTable:
    dc.l    .SDCARD_INIT
    dc.l    .SDCARD_RD_BLK
    dc.l    .SDCARD_RD_BLK_N
    dc.l    .SDCARD_WR_BLK
    dc.l    .SDCARD_WR_BLK_N
    dc.l    .XMODEMRECV
    dc.l    MFPRECV                     ; call into direct
    dc.l    MFPSEND                     ; call into direct
    dc.l    MFPBYTEAVAIL                ; call into direct
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

; D0: starting block number to read from
; D1: count of blocks to read (no longer in D1 but originally should be. get off stack instead)
; A0: pointer to allocated 512 * count byte buffer
; A1: pointer to allocated 1 byte buffer (no longer in A1 but originally should be. get off stack instead)
.SDCARD_RD_BLK_N:
    move.l  4(SP),-(SP)                 ; copy original A1 onto the stack
    move.l  A0,-(SP)
    move.l  16(SP),-(SP)                ; copy original D1 onto the stack (careful bc we've added to the stack twice!)
    move.l  D0,-(SP)
    jsr     sdcard_read_block_n
    add.l   #16,SP
    rts

; D0: block number to write to
; A0: pointer to source 512 byte buffer
; A1: pointer to 1 byte buffer (no longer in A1 but originally should be. get off stack instead)
.SDCARD_WR_BLK:
    move.l  4(SP),-(SP)                 ; copy original A1 onto the stack
    move.l  A0,-(SP)
    move.l  D0,-(SP)
    jsr     sdcard_write_block
    add.l   #12,SP
    rts

; D0: starting block number to read from
; D1: count of blocks to read (no longer in D1 but originally should be. get off stack instead)
; A0: pointer to allocated 512 * count byte buffer
; A1: pointer to allocated 1 byte buffer (no longer in A1 but originally should be. get off stack instead)
.SDCARD_WR_BLK_N:
    move.l  4(SP),-(SP)                 ; copy original A1 onto the stack
    move.l  A0,-(SP)
    move.l  16(SP),-(SP)                ; copy original D1 onto the stack (careful bc we've added to the stack twice!)
    move.l  D0,-(SP)
    jsr     sdcard_write_block_n
    add.l   #16,SP
    rts

; D0: maxsize of the destination buffer
; A0: pointer to buffer that is at least `maxsize` size
.XMODEMRECV:
    move.l  D0,-(SP)
    move.l  A0,-(SP)
    jsr     xmodem_recv
    add.l   #8,SP
    rts
