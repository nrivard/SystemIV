#pragma once

#include <stdint.h>

#define swap_endian32(num)  __builtin_bswap32(num)
// #define swap_endian32(num)  (((num & 0xFF) << 24) + ((num & 0xFF00) << 8) + ((num & 0xFF0000) >> 8) + ((num & 0xFF000000) >> 24))

typedef enum {
    FAT_NOERR = 0,
    FAT_ERROR_SDCARD,   // no sdcard found
    FAT_NOT_FAT,        // not a valid FAT filesystem
} fat_error_t;

typedef struct {
    uint8_t bootFlag;
    uint8_t type;
    uint32_t lba;
    uint32_t sectorCount;
} __attribute__((packed)) fat_partition_t;

typedef struct {
    fat_partition_t partitions[4];
} fat_volume_t;

fat_error_t fat_init(fat_volume_t *volume);
