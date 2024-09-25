
;; this code is intended to reside in the MBR of a FAT partitioned bootable device
;; It will get loaded and executed from the top at $00002000. It is intended to:
;; 
;; 1. find a FAT formatted partition
;; 2. calculate location of root dir in that partition
;; 3. look for "SYSTEMIV.BIN" file in root dir, copy it, and execute it
;;
;; NOTE: for now, for size and complexity reasons, this will only look in the first cluster
;; of the root dir for the intended file AND it will only load a file that is smaller than one cluster.
;; this does no FAT table chaining to search or copy files larger than a cluster
    public START

    section .text.init

FAT_SIG_OFFSET          equ $1FE
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
    
; arg1: base-pointer to number you want to convert
; arg2: offset into base-pointer you want to convert
; arg3: destination for 16 bit number
FAT_GET_16 macro
    move.b  \2+1(\1),\3
    ror.w   #8,\3
    move.b  \2(\1),\3
    endm
    
; arg1: base-pointer to number you want to convert
; arg2: offset into base-pointer you want to convert
; arg3: destination for 16 bit number
FAT_GET_32 macro
    move.b  \2+3(\1),\3
    ror.w   #8,\3
    move.b  \2+2(\1),\3
    swap    \3
    move.b  \2+1(\1),\3
    ror.w   #8,\3
    move.b  \2(\1),\3
    endm

START:
    lea     RUN_START,A6        ; start of this sector
    clr.l   D7                  ; progress in booting
.VerifySignature:
    cmp.w   #$55AA,FAT_SIG_OFFSET(A6)
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
    addq.l  #1,D7

    move.l  FAT_MBR_PART_LBA(A5),D3     ; little-endian long of LBA
    ENDIAN32 D3                         ; block #
    move.l  D3,VOLUMEID                 ; save start of volume
    clr.l   D4                          ; index should be 0 here
    jsr     NEXTSECTOR
    bne     ERROR
    addq.l  #1,D7
    
.VerifyVolume:
    lea     SECTORDATA,A4        ; volume pointer
    cmp.w   #$55AA,(A4)
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
    asl.l   #5,D1                        ; multiply by size of fat_record_t
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

    addq.l  #1,D7                       ; debug. did we get here?

.ReadRoot:
    move.l  VOLUMEROOT,D3
    clr.l   D4                          ; sector count. if our file isn't in first cluster return an error
.FetchSector:
    jsr     NEXTSECTOR
    bne     ERROR
    addq.l  #1,D4                       ; inc index

.ReadDirLoop:
    lea     SECTORDATA,A0
.IterateFileLoop:
    cmp.l   SECTORDATAEND,A0
    bge     .FetchSector
.CheckFile:
    cmp.b   #0,(A0)                     ; first byte a zero? end of dir with no matches :(
    beq     ERROR
    cmp.b   #$E5,(A0)                   ; first byte $E5? unused.
    beq     .NextFile
.CheckName:
    move.b  #(SYSIVEND-SYSIV-1),D1      ; compare filename to SYSTEMIV.BIN
    move.l  A0,A1                       ; copy ptr to A1
    lea     SYSIV,A2
.NameLoop:
    cmp.b   (A1)+,(A2)+
    bne     .NextFile
    dbra    D1,.NameLoop

.Found:
    move.l  FAT_DIR_SIZE(A0),D5
    ENDIAN32 D5                         ; get size of the file

    move.w  FAT_DIR_CLSTR_HI(A0),D3     ; fetch hi cluster
    ENDIAN16 D3
    swap    D3
    move.w  FAT_DIR_CLSTR_LO(A0),D3     ; fetch lo cluster
    ENDIAN16 D3

.Cluster2Lba:
    subq.l  #2,D3                       ; ((clstr - 2) * clusters_per_sector) + cluster_start
    move.b  VOLUMESECS,D0               ; can't use mulu bc that is 16bit * 16bit. Cluster can be larger than 16 bit
.MultLoop:                              ; so take advantage of the fact that sectors-per-cluster is always a power of 2
    asr.b   #1,D0
    beq     .MultDone
    asl.l   #1,D3
    bra     .MultLoop
.MultDone:
    add.l   VOLUMEROOT,D3

.FetchFile:
    clr.l   D4                          ; current sector
    movea.l KERN_DEST,A6                ; copy destination
.FetchFileLoop:
    jsr     NEXTSECTOR
    move.l  A4,A5                       ; copy SECTORDATA ptr
    move.w  #FAT_SECTOR_SZ-1,D2         ; size of each sector
.CopyFileLoop:
    move.b  (A5)+,(A6)+
    subq.l  #1,D5                       ; sub from size of the file
    beq     .RunKernel
    dbra    D2,.CopyFileLoop
.RunKernel:
    jmp     KERN_DEST                   ; jump into kernel land and hope for the best

.NextFile:
    add.l   #32,A0
    bra     .IterateFileLoop


; move error code into D0
ERROR:
    move.l  D7,D0
    rts

; fetches next sector until it runs out of available sectors
; D3: starting sector of cluster
; D4: index of sector in cluster to fetch
; data will be put in SECTORDATA buffer
; relies on VOLUME* variables being setup properly
; returns zero in D0 if data was fetched, nonzero otherwise
NEXTSECTOR:
    cmp.b   VOLUMESECS,D4
    bge     .Error                      ; run out of sectors to fetch
    lea     SECTORDATA,A0
    lea     TOKEN,A1
    move.l  D3,D0
    add.l   D4,D0                       ; sector number to fetch
    moveq.l #1,D1                       ; sdcard_read_block
    trap    #13
    tst.l   D0                          ; SDCARD_NOERR?
    bra     .Done                       ; D0 is already correct
.Error:
    moveq.l #1,D0
.Done:
    rts
    

SYSIV:  dc.b "SYSTEMIVBIN"
SYSIVEND:
    
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

SECTORDATA      equ     START+FAT_SECTOR_SZ
SECTORDATAEND   equ     SECTORDATA+FAT_SECTOR_SZ

VOLUMEID        equ     SECTORDATAEND   ; long
VOLUMEFAT       equ     VOLUMEID+4      ; long
VOLUMEROOT      equ     VOLUMEFAT+4     ; long
VOLUMETYPE      equ     VOLUMEROOT+4    ; byte (TODO: eliminate storing this to save space!)
VOLUMESECS      equ     VOLUMETYPE+1    ; byte

TOKEN           equ     VOLUMESECS+1    ; byte 
