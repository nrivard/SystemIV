#include <stdint.h>

#include "fat.h"
#include "sdcard.h"

#define FAT_SECTOR_SIGNATURE_1  0x55
#define FAT_SECTOR_SIGNATURE_2  0xAA
#define FAT_PARTITION_OFFSET    0x1BE

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

    fat_partition_t *partition = (fat_partition_t *)&block[FAT_PARTITION_OFFSET];
    for (int i = 0; i < 4; i++) {
        volume->partitions[i] = *partition++;
    }

    return FAT_NOERR;
}
