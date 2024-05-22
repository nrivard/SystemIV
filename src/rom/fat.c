#include <stdint.h>
#include <string.h>

#include "fat.h"
#include "sdcard.h"

#define FAT_SECTOR_SIGNATURE_1      0x55
#define FAT_SECTOR_SIGNATURE_2      0xAA

#define FAT_MBR_PARTITION_FAT32     0x0B
#define FAT_MBR_PARTITION_FAT32_LBA 0X0C
#define FAT_MBR_PARTITION_FAT16     0x0E

#define FAT_VALID_SECTOR(block)     (block[0x1FE] == FAT_SECTOR_SIGNATURE_1 && block[0x1FF] == FAT_SECTOR_SIGNATURE_2)

// NOTE! All multi-byte numbers for on-disk records are LITTLE ENDIAN

typedef struct {
    uint8_t bootFlag;
    uint8_t chsBegin[3];
    uint8_t type;
    uint8_t chsEnd[3];
    uint32_t lba;
    uint32_t sectorCount;
} __attribute__((packed)) fat_mbr_partition_t;

// you are not supposed to allocate this struct, but rather cast your MBR block as a pointer :)
typedef struct {
    uint8_t bootCode[446];
    fat_mbr_partition_t partitions[4];
    uint16_t signature;
} __attribute__((packed)) fat_mbr_t;

typedef struct {
    uint8_t bootstrap[3];
    char systemID[8];
    uint16_t sectorSize;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t fats;
    uint16_t rootEntries;   // always zero for FAT32
    uint16_t smallSectors;  // always zero for FAT32
    uint8_t mediaDescriptor;
    uint16_t sectorsPerFat16; // always zero for FAT32

} __attribute((packed)) fat_volume_id_t;

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

// fetches volume ID sector and completes initialization for fat32 volume
fat_error_t fat_init_32(fat_volume_t *volume, uint32_t lba);

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
        return FAT_ERROR_BAD_SECTOR;
    }

    fat_mbr_t *mbr = (fat_mbr_t *)block;

    // copy partition information that we care about
    for (int i = 0; i < 4; i++) {
        fat_mbr_partition_t *from = &mbr->partitions[i];
        fat_volume_t *to = &disk->volumes[i];

        // prefill out lba even if it's invalid
        uint32_t lba = swap_endian32(from->lba);
        
        switch (from->type) {
            case FAT_MBR_PARTITION_FAT16:
                to->type = FAT_16;
                break;

            case FAT_MBR_PARTITION_FAT32:
            case FAT_MBR_PARTITION_FAT32_LBA:
                fat_error_t error = fat_init_32(to, lba);
                to->type = error == FAT_NOERR ? FAT_32 : FAT_NOT_FAT;
                break;

            default:
                to->type = FAT_NOT_FAT;
        }
    }

    return FAT_NOERR;
}

fat_error_t fat_init_32(fat_volume_t *volume, uint32_t lba) {
    uint8_t block[512];
    uint8_t token;
    if (sdcard_read_block(lba, block, &token) != SDCARD_NOERR) {
        return FAT_ERROR_SDCARD;
    }

    if (!FAT_VALID_SECTOR(block)) {
        return FAT_ERROR_BAD_SECTOR;
    }

    uint16_t *rawSectorSize = (uint16_t *)&block[0x0B];
    uint16_t sectorSize = swap_endian16(*rawSectorSize);
    if (sectorSize != 512) {
        return FAT_ERROR_SECTOR_SIZE;
    }

    if (block[0x10] != 2) {
        return FAT_ERROR_NUM_FATS;
    }

    

    return FAT_NOERR;
}
