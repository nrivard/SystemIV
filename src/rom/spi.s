    section .text

; change these if your setup is different
SPI_PORT    equ     MFP_GPIP
SPI_DDR     equ     MFP_DDR

SPI_CLK_BIT equ     6
SPI_MISO_BIT equ    7

SPI_CLK::   equ     (1<<SPI_CLK_BIT)
SPI_CS::    equ     (1<<5)
SPI_MISO::  equ     (1<<SPI_MISO_BIT)
SPI_MOSI::  equ     (1<<4)

SPI_CLK_MSK  equ    (SPI_CLK^$FF)
SPI_MOSI_MSK equ    (SPI_MOSI^$FF)
SPI_MISO_MSK equ    (SPI_MISO^$FF)

; by default: CS, CLK, and MOSI low (default to `0` for bit to send)
SPI_PRE_BIT_MSK:: equ (SPI_MOSI_MSK&SPI_CLK_MSK)

; C interface to initialization
spi_init::
; Sets pin roles and initial values
; destroys:
;   D0
SPIINIT::
    move.b  SPI_DDR,D0
    or.b    #(SPI_CLK|SPI_CS|SPI_MOSI),D0
    andi.b  #(SPI_MISO_MSK),D0
    move.b  D0,SPI_DDR

    move.b  SPI_PORT,D0
    or.b    #(SPI_CLK|SPI_CS|SPI_MOSI),D0
    move.b  D0,SPI_PORT

; C interface to transfer the passed in byte and return received byte
spi_transfer::
    move.l  4(SP),D0
    ; fallthrough

; params:
;   D0: the byte to send to the device
; returns:
;   D0: the received byte
; destroys:
;   D0,D1
SPITX:
    movem.l D2-D3,-(SP)
    move.l  D0,D1               ; D1 holds value to send
    clr.l   D0                  ; D0 holds return value
    moveq.l #7,D3               ; our index register
.BitLoop:
    move.b  SPI_PORT,D2         ; SPI port values
    andi.b  #SPI_PRE_BIT_MSK,D2   ; start with clk and mosi low
    asl.b   #1,D1               ; shift MSB into carry
    bcc     .SendBit
    or.b    #SPI_MOSI,D2      ; MSB is a `1`
.SendBit:
    move.b  D2,SPI_PORT
    rept 2
    nop                         ; even out duty cycle
    endr
    bset    #(SPI_CLK_BIT),D2   ; flip clk bit back high
    move.b  D2,SPI_PORT
.ReadBit:
    move.b  SPI_PORT,D2         ; read port back into D2
    roxl.b  #(8-SPI_MISO_BIT),D2 ; rotate MISO into extend bit
    roxl.b  #1,D0               ; rotate MISO into return value
    dbra    D3,.BitLoop
.Done:
    move.b  SPI_PORT,D2
    or.b    #(SPI_MOSI),D2 ; MOSI high (CLK should already be high)
    move.b  D2,SPI_PORT
    movem.l (SP)+,D2-D3
    rts
