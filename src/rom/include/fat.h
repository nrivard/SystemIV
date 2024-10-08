#pragma once

#include <stdint.h>
#include <stdbool.h>

#define swap_endian32(num)  __builtin_bswap32(num)
#define swap_endian16(num)  __builtin_bswap16(num)

#define FAT_GET_16(buffer, offset)  (((uint16_t)buffer[offset + 1] << 8) | ((uint16_t)buffer[offset]))
#define FAT_GET_32(buffer, offset)  (((uint32_t)buffer[offset + 3] << 24) | ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 1] << 8) | ((uint32_t)buffer[offset]))

#define FAT_SECTOR_SIZE     512

typedef enum {
    FAT_NOERR = 0,
    FAT_ERROR_SDCARD,       // no sdcard found
    FAT_ERROR_BAD_SECTOR,   // sector signature check failed
    FAT_ERROR_SECTOR_SIZE,  // invalid sector size. should be 512
    FAT_ERROR_NUM_FATS      // invalid number of tables. should be 2
} fat_error_t;

typedef enum {
    FS_UNKNOWN = 0,
    FS_FAT16,
    FS_FAT32
} fs_type_t;

typedef struct {
    fs_type_t   type;
    uint8_t     sectorsPerCluster; // sectors per cluster
    uint32_t    volumeSector;   // first sector of partition
    uint32_t    fatSector;      // sector of first FAT
    uint32_t    dataSector;  // sector of the first cluster
    uint32_t    rootCluster;    // cluster of root dir
} fat_volume_t;

typedef struct {
    fat_volume_t volumes[4];
} fat_disk_t;

fat_error_t fat_init(fat_disk_t *disk);
fat_error_t fat_read(fat_volume_t *volume, uint32_t cluster, uint8_t block[FAT_SECTOR_SIZE], bool (*process)(uint8_t block[FAT_SECTOR_SIZE]));