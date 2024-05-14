#pragma once

#include <stdint.h>

typedef enum {
    FAT_NOERR = 0,
    FAT_ERROR_SDCARD,   // no sdcard found
    FAT_NOT_FAT,        // not a valid FAT filesystem
} fat_error_t;

typedef struct {
    uint8_t bootFlag;
    uint8_t chsBegin[3];
    uint8_t type;
    uint8_t chsEnd[3];
    uint32_t lba;
    uint32_t sectorCount;
} __attribute__((packed)) fat_partition_t;

typedef struct {
    fat_partition_t partitions[4];
} fat_volume_t;

fat_error_t fat_init(fat_volume_t *volume);
