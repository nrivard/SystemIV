#pragma once

#include <stdint.h>

#define swap_endian32(num)  __builtin_bswap32(num)
#define swap_endian16(num)  __builtin_bswap16(num)
// #define swap_endian32(num)  (((num & 0xFF) << 24) + ((num & 0xFF00) << 8) + ((num & 0xFF0000) >> 8) + ((num & 0xFF000000) >> 24))

typedef enum {
    FAT_NOERR = 0,
    FAT_ERROR_SDCARD,       // no sdcard found
    FAT_ERROR_BAD_SECTOR,   // sector signature check failed
    FAT_ERROR_SECTOR_SIZE,  // invalid sector size. should be 512
    FAT_ERROR_NUM_FATS      // invalid number of tables. should be 2
} fat_error_t;

typedef enum {
    FAT_NOT_FAT = 0,
    FAT_16,
    FAT_32
} fat_type_t;

typedef struct {
    uint32_t    fatSector;      // sector of first FAT
    uint32_t    rootCluster;    // sector of root dir
    fat_type_t  type;
    uint8_t     sectorsPerCluster;
} __attribute__((packed)) fat_volume_t;

typedef struct {
    fat_volume_t volumes[4];
} fat_disk_t;

fat_error_t fat_init(fat_disk_t *disk);
