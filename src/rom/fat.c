#include <stdint.h>
#include <string.h>

#include "fat.h"
#include "sdcard.h"

#define FAT_SECTOR_SIGNATURE_1      0x55
#define FAT_SECTOR_SIGNATURE_2      0xAA
#define FAT_PARTITION_OFFSET        0x1BE

#define FAT_MBR_PARTITION_FAT32     0x0B
#define FAT_MBR_PARTITION_FAT32_LBA 0X0C
#define FAT_MBR_PARTITION_FAT16     0x0E

#define FAT_VALID_SECTOR(block)     (block[0x1FE] == FAT_SECTOR_SIGNATURE_1 && block[0x1FF] == FAT_SECTOR_SIGNATURE_2)

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

// fetches volume ID sector and completes initialization for fat32 volume
fat_error_t fat_init_32(fat_volume_t *volume);

fat_error_t fat_init(fat_disk_t *disk) {
    memset(disk, 0, sizeof(fat_volume_t));

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

    if (!FAT_VALID_SECTOR(block)) {
        return FAT_BAD_SECTOR;
    }

    // cache partition data
    fat_mbr_partition_t mbrparts[4];
    memcpy(mbrparts, &block[FAT_PARTITION_OFFSET], 4 * sizeof(fat_mbr_partition_t));

    // copy partition information that we care about
    for (int i = 0; i < 4; i++) {
        fat_mbr_partition_t *from = &mbrparts[i];
        fat_volume_t *to = &disk->volumes[i];

        // prefill out lba even if it's invalid
        to->lba = swap_endian32(from->lba);
        
        switch (from->type) {
            case FAT_MBR_PARTITION_FAT16:
                to->type = FAT_16;
                break;

            case FAT_MBR_PARTITION_FAT32:
            case FAT_MBR_PARTITION_FAT32_LBA:
                to->type = FAT_32;

                // TODO: do we need to check if errors here?
                fat_init_32(to);
                break;

            default:
                to->type = FAT_NOT_FAT;
        }
    }

    return FAT_NOERR;
}

fat_error_t fat_init_32(fat_volume_t *volume) {
    if (!volume->lba) {
        return FAT_NOT_FAT;
    }

    uint8_t block[512];
    uint8_t token;
    if (sdcard_read_block(volume->lba, block, &token) != SDCARD_NOERR) {
        return FAT_ERROR_SDCARD;
    }

    

    return FAT_NOERR;
}
