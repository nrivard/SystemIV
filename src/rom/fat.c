#include <stdint.h>

#include "fat.h"
#include "sdcard.h"

#define FAT_SECTOR_SIGNATURE_1      0x55
#define FAT_SECTOR_SIGNATURE_2      0xAA
#define FAT_PARTITION_OFFSET        0x1BE

#define FAT_MBR_PARTITION_FAT32     0x0B
#define FAT_MBR_PARTITION_FAT32_LBA 0X0C
#define FAT_MBR_PARTITION_FAT16     0x0E

// NOTE! All multi-byte numbers for on-disk records are LITTLE ENDIAN

typedef struct {
    char filename[8];
    char extension[3];
    uint8_t attributes;
    uint8_t RESERVED;
    uint8_t creationCentisec;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t lastAccessDate;
    uint16_t clusterHigh;
    uint16_t writeTime;
    uint16_t writeDate;
    uint16_t clusterLow;
    uint32_t size;
} __attribute__((packed)) fat_record_t;

typedef struct {
    uint8_t bootFlag;
    uint8_t chsBegin[3];
    uint8_t type;
    uint8_t chsEnd[3];
    uint32_t lba;
    uint32_t sectorCount;
} __attribute__((packed)) fat_mbr_partition_t;

fat_error_t fat_init(fat_volume_t *volume) {
    sdcard_device_t device;
    if (sdcard_init(&device) != SDCARD_NOERR || device.status != SDCARD_STATUS_READY) {
        return FAT_ERROR_SDCARD;
    }

    // fetch MBR
    uint8_t block[512];
    uint8_t token;
    if (sdcard_read_block(0, block, &token) != SDCARD_NOERR) {
        return FAT_ERROR_SDCARD;
    }

    if (block[0x1FE] != FAT_SECTOR_SIGNATURE_1 || block[0x1FF] != FAT_SECTOR_SIGNATURE_2) {
        return FAT_NOT_FAT;
    }

    // copy partition information that we care about
    fat_mbr_partition_t *mbrparts = (fat_mbr_partition_t *)&block[FAT_PARTITION_OFFSET];
    for (int i = 0; i < 4; i++) {
        fat_mbr_partition_t *from = &mbrparts[i];
        
        // for now only support FAT16
        if (from->type != FAT_MBR_PARTITION_FAT16) {
            continue;
        }

        fat_partition_t *to = &volume->partitions[i];

        to->bootFlag = from->bootFlag;
        to->type = from->type;
        to->lba = from->lba;
        to->sectorCount = swap_endian32(from->sectorCount);
    }

    return FAT_NOERR;
}
