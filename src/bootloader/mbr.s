
;; this code is intended to reside in the MBR of a FAT partitioned bootable device
;; It will get loaded and executed from the top.
    public START

    section .text.init

FAT_SIG_OFFSET_1        equ $1FE
FAT_SIG_OFFSET_2        equ $1FF
FAT_SECTOR_SZ           equ $200

FAT_MBR_PART_TABLE      equ $01BE

FAT_MBR_PART_TYPE       equ $04     ; offset from start of partition entry to type field
FAT_MBR_PART_LBA        equ $08

FAT_VOL_BPS             equ $0B
FAT_VOL_SEC_PER_CLSTR   equ $0D
FAT_VOL_RES             equ $0E
FAT_VOL_FATS            equ $10
FAT_VOL_RT_ENTRIES      equ $11
FAT_VOL_FAT_16          equ $16
FAT_VOL_HIDDEN          equ $1C
FAT_VOL_FAT_32          equ $24
FAT_VOL_RT_CLSTR        equ $2C

FAT_DIR_FILENAME        equ $00
FAT_DIR_ATTR            equ $0B
FAT_DIR_CLSTR_HI        equ $14
FAT_DIR_CLSTR_LO        equ $1A
FAT_DIR_SIZE            equ $1C
        
; arg: register containing lower word to convert endianness
ENDIAN16 macro
    ror.w #8,\1
    endm

; arg: register containing long word to convert endianness
ENDIAN32 macro
    ror.w #8,\1
    swap \1
    ror.w #8,\1
    endm
    
FAT_GET_16 macro
    move.b  \2+1(\1),\3
    ror.w   #8,\3
    move.b  \2(\1),\3
    endm
    
FAT_GET_32 macro
    move.b  \2+3(\1),\3
    ror.w   #8,\3
    move.b  \2+2(\1),\3
    swap    \3
    move.b  \2+1(\1),\3
    ror.w   #8,\3
    move.b  \2(\1),\3
    endm

;; 1. find a partition
;; 2. copy first sector of that partition
;; 3. execute boot code in that sector
START:
    lea     RUN_START,A6        ; start of this sector
    clr.l   D7                  ; progress in booting
.VerifySignature:
    cmp.b   #$55,FAT_SIG_OFFSET_1(A6)
    bne     ERROR
    cmp.b   #$AA,FAT_SIG_OFFSET_2(A6)
    bne     ERROR
    addq.l  #1,D7

; only reading the first partition!
.ReadPartitionTable:
    lea     FAT_MBR_PART_TABLE(A6),A5   ; partition entry
    cmp.b   #$0B,FAT_MBR_PART_TYPE(A5)
    beq     .ReadPartitionLBA
    cmp.b   #$0C,FAT_MBR_PART_TYPE(A5)
    bne     ERROR

.ReadPartitionLBA:
    move.b  FAT_MBR_PART_TYPE(A5),VOLUMETYPE    ; TODO: can eliminate!
    addq.l  #1,D7

    move.l  FAT_MBR_PART_LBA(A5),D0     ; little-endian long of LBA
    ENDIAN32 D0                         ; block #
    move.l  D0,VOLUMEID                 ; save start of volume

    lea     BOOTVOLUME,A0
    lea     TOKEN,A1
    move.l  #1,D1                       ; sdcard_read_block
    trap    #13
    tst.l   D0                          ; SDCARD_NOERR?
    bne     ERROR
    addq.l  #1,D7
    
.VerifyVolume:
    lea     FAT_SECTOR_SZ(A6),A4        ; volume pointer
    cmp.b   #$55,FAT_SIG_OFFSET_1(A4)
    bne     ERROR
    cmp.b   #$AA,FAT_SIG_OFFSET_2(A4)
    bne     ERROR
    cmp.b   #$00,FAT_VOL_BPS(A4)        ; lower byte of $0200 (little-endian)
    bne     ERROR
    cmp.b   #$02,FAT_VOL_BPS+1(A4)      ; upper byte of $0200 (little-endian)
    bne     ERROR
    cmp.b   #$02,FAT_VOL_FATS(A4)       ; num of fats should be 2
    bne     ERROR
    addq.l  #1,D7
    
.ReadVolume:
    FAT_GET_16  A4,FAT_VOL_RES,D0       ; D0: reserved (still 0 from sdcard_read_block)
    
    FAT_GET_16  A4,FAT_VOL_RT_ENTRIES,D1 ; D1: rootDirSectors
    mulu    #32,D1                      ; multiply by size of fat_record_t
    add.l   #FAT_SECTOR_SZ-1,D1
    divu    #FAT_SECTOR_SZ,D1
    and.l   #$FFFF,D1                   ; clear upper word junk
    
    clr.l   D2
    FAT_GET_16  A4,FAT_VOL_FAT_16,D2    ; D2: fatSize
    bne     .NonZero                    ; FAT16?
    FAT_GET_32  A4,FAT_VOL_FAT_32,D2
.NonZero
    asl.l   #1,D2                       ; multiply by 2 as there are 2 FATs
    
.FindDataSector:
    add.l   D0,D1                       ; D1 is now our data sector LBA (we still need reserved so not using D0)
    add.l   D2,D1
    
.SaveVolume:
    move.b  FAT_VOL_SEC_PER_CLSTR(A4),VOLUMESECS   ; sectorsPerCluster
    add.l   VOLUMEID,D0                 ; fatSector
    move.l  D0,VOLUMEFAT
    add.l   VOLUMEID,D1                 ; dataSector
    move.l  D1,VOLUMEROOT
    FAT_GET_32  A4,FAT_VOL_RT_CLSTR,D0
    move.l  D0,VOLUMECLSTR

    addq.l  #1,D7                       ; debug. did we get here?

.ReadRoot:
    clr.l   D6                          ; sector count. if our file isn't in first sector we have failed
.FetchSector:
    cmp.l   VOLUMESECS,D6
    bge     ERROR                       ; run out of sectors to fetch
    lea     ROOTSECTOR,A0
    lea     TOKEN,A1
    move.l  VOLUMEROOT,D0
    add.l   D6,D0                       ; sector number to fetch
    moveq.l #1,D1                       ; sdcard_read_block
    trap    #13
    tst.l   D0                          ; SDCARD_NOERR?
    bne     ERROR

; move error code into D0
ERROR:
    move.l  D7,D0
    rts
    
; padding:
;     cnop    $EA,FAT_MBR_PART_TABLE         ; padding up to where the partition table will be

;     ds.b    FAT_SECTOR_SZ-FAT_MBR_PART_TABLE    ; reserve space for where the rest of the sector will be
    
;         ORG $11BE

; PARTENTRY1: ds.b    16 ;$00, $82, $03, $00, $0c, $fe, $ff, $ff, $00, $20, $00, $00, $00, $04, $b7, $03
; PARTENTRY2: ds.b    16
; PARTENTRY3: ds.b    16
; PARTENTRY4: ds.b    16

;     dc.b    $55
;     dc.b    $AA

BOOTVOLUME      equ     START+FAT_SECTOR_SZ
ROOTSECTOR      equ     BOOTVOLUME+FAT_SECTOR_SZ

VOLUMEID        equ     ROOTSECTOR+FAT_SECTOR_SZ ; long
VOLUMEFAT       equ     VOLUMEID+4      ; long
VOLUMEROOT      equ     VOLUMEFAT+4     ; long
VOLUMECLSTR     equ     VOLUMEROOT+4    ; long
VOLUMETYPE      equ     VOLUMECLSTR+4   ; byte (TODO: eliminate storing this to save space!)
VOLUMESECS      equ     VOLUMETYPE+1    ; byte

TOKEN           equ     VOLUMESECS+1    ; byte 
