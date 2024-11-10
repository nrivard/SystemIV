    section .text

; change these if your setup is different
SPI_PORT        equ     MFP_GPIP
SPI_DDR         equ     MFP_DDR

SPI_CLK_BIT     equ     6
SPI_MISO_BIT    equ     7

SPI_CLK         equ     (1<<SPI_CLK_BIT)
SPI_CS          equ     (1<<5)
SPI_MISO        equ     (1<<SPI_MISO_BIT)
SPI_MOSI        equ     (1<<4)

SPI_CS_MSK      equ    (SPI_CS^$FF)
SPI_CLK_MSK     equ    (SPI_CLK^$FF)
SPI_MOSI_MSK    equ    (SPI_MOSI^$FF)
SPI_MISO_MSK    equ    (SPI_MISO^$FF)


; Sets pin roles and initial values
; destroys:
;   D0
SPIINIT::
    move.b  SPI_DDR,D0              ; set port directions
    or.b    #(SPI_CLK|SPI_CS|SPI_MOSI),D0
    andi.b  #(SPI_MISO_MSK),D0
    move.b  D0,SPI_DDR

    move.b  SPI_PORT,D0             ; set port values
    or.b    #(SPI_CS|SPI_MOSI),D0   ; CLK low, CS and MOSI high
    move.b  D0,SPI_PORT
    rts

; C interface to transfer the passed in byte and return received byte
SPITXC::
    move.l  4(SP),D0
    ;; fallthrough

; ASM and TRAP interface to transfer passed in byte and return received byte
; params:
;   D0: the byte to send to the device
; returns:
;   D0: the received byte
; destroys:
;   D0
SPITX::
    movem.l D1-D3,-(SP)
    move.l  D0,D1               ; D1 holds value to send
    clr.l   D0                  ; D0 holds return value
    moveq.l #7,D3               ; our index register
.BitLoop:
    move.b  SPI_PORT,D2         ; SPI port values
    ori.b   #(SPI_MOSI),D2      ; MOSI high (many SD ops are $FF!)
    add.b   D1,D1               ; shift MSB into carry (saves 4 cycles over rotating)
    bcs     .SendBit
    andi.b  #(SPI_MOSI_MSK),D2  ; MSB is a `0`
.SendBit:
    move.b  D2,SPI_PORT         ; set value before CLK
    ori.b   #(SPI_CLK),SPI_PORT ; flip clock high directly on the port
.ReadBit:
    move.b  SPI_PORT,D2         ; read port back into D2
    add.b   D2,D2               ; shift MISO into extend bit (saves 4 cycles over rotating)
    roxl.b  #1,D0               ; rotate extend bit into return value
.ToggleClock:
    eori.b  #(SPI_CLK),SPI_PORT ; flip clock low directly on the port
    dbra    D3,.BitLoop
.Done:
    or.b    #(SPI_MOSI),SPI_PORT ; MOSI high (CLK should already be low)
    movem.l (SP)+,D1-D3
    rts

SPIASSERT::
    andi.b  #SPI_CS_MSK,SPI_PORT
    rts

SPIDEASSERT::
    ori.b   #SPI_CS,SPI_PORT
    rts
