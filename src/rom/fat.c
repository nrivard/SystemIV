#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "fat.h"
#include "sdcard.h"

#include "serial.h"

#define FAT_SECTOR_SIGNATURE_1      0x55
#define FAT_SECTOR_SIGNATURE_2      0xAA

#define FAT_MBR_PARTITION_FAT32     0x0B
#define FAT_MBR_PARTITION_FAT32_LBA 0X0C
#define FAT_MBR_PARTITION_FAT16     0x0E

#define FAT_VOLUME_ID_SECTOR_SZ     0x0B
#define FAT_VOLUME_ID_SEC_PER_CLSTR 0x0D
#define FAT_VOLUME_ID_RES_SECTORS   0x0E
#define FAT_VOLUME_ID_FATS          0x10
#define FAT_VOLUME_ID_ROOT_ENTRIES  0x11
#define FAT_VOLUME_ID_SECTORS_16    0x13
#define FAT_VOLUME_ID_FAT_SZ_16     0x16
#define FAT_VOLUME_ID_SECTORS_32    0x20
#define FAT_VOLUME_ID_FAT_SZ_32     0x24
#define FAT_VOLUME_ID_ROOT_CLSTR    0x2C

const char *fatz = "fats";

#define FAT_VALID_SECTOR(block)     (block[0x1FE] == FAT_SECTOR_SIGNATURE_1 && block[0x1FF] == FAT_SECTOR_SIGNATURE_2)

#define DEBUG_BYTE(byte)            serial_put_hex(byte); \
                                    serial_put(' ');

#define DEBUG_LONG(val)             serial_put_long(val); \
                                    serial_put(' ');
                                
#define DEBUG_KEY_BYTE(key, byte)  serial_put_string(key); \
                                   serial_put_string(": "); \
                                   serial_put_hex(byte); \
                                   serial_put_string("\r\n");

#define DEBUG_KEY_LONG(key, lng)  serial_put_string(key); \
                                   serial_put_string(": "); \
                                   serial_put_long(lng); \
                                   serial_put_string("\r\n");

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
// NOTE: FAT volume ID is not structured this way bc it has many word or long values at odd addresses!
// this is a nightmare to try and work out
typedef struct {
    uint8_t bootCode[446];
    fat_mbr_partition_t partitions[4];
    uint16_t signature;
} __attribute__((packed)) fat_mbr_t;

typedef struct {
    char filename[11];
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

static inline uint32_t fat_sector(fat_volume_t *volume, uint32_t cluster) {
    return volume->dataSector + ((cluster - 2) * volume->sectorsPerCluster);
}

fat_error_t fat_init(fat_disk_t *disk) {
    memset(disk, 0, sizeof(fat_volume_t));

    sdcard_device_t device;
    if (sdcard_init(&device) != SDCARD_NOERR || device.status != SDCARD_STATUS_READY) {
        return FAT_ERROR_SDCARD;
    }

    // fetch MBR
    uint8_t block[FAT_SECTOR_SIZE];
    uint8_t token;
    if (sdcard_read_block(0, block, &token) != SDCARD_NOERR) {
        return FAT_ERROR_SDCARD;
    }

    if (!FAT_VALID_SECTOR(block)) {
        return FAT_ERROR_BAD_SECTOR;
    }

    // copy the partitions so we can re-use the block buffer
    fat_mbr_t *mbr = (fat_mbr_t *)block;
    fat_mbr_partition_t partitions[4];
    memcpy(partitions, mbr->partitions, sizeof(fat_mbr_partition_t) * 4);

    // initialize each volume
    for (int i = 0; i < 4; i++) {
        fat_mbr_partition_t *from = partitions + i;
        fat_volume_t *to = disk->volumes + i;

        // is this a FAT partition?
        uint8_t type = from->type;
        if (type != FAT_MBR_PARTITION_FAT16 && type != FAT_MBR_PARTITION_FAT32 && type != FAT_MBR_PARTITION_FAT32_LBA) {
            continue;
        }

        // fetch volume ID
        to->volumeSector = swap_endian32(from->lba);

        if (sdcard_read_block(to->volumeSector, block, &token) != SDCARD_NOERR) {
            continue;
        }

        if (!FAT_VALID_SECTOR(block)) {
            continue;
        }

        if (FAT_GET_16(block, FAT_VOLUME_ID_SECTOR_SZ) != FAT_SECTOR_SIZE) {
            continue;
        }

        if (block[FAT_VOLUME_ID_FATS] != 2) {
            continue;
        }

        uint16_t reserved = FAT_GET_16(block, FAT_VOLUME_ID_RES_SECTORS);
        uint32_t rootDirSectors = ((FAT_GET_16(block, FAT_VOLUME_ID_ROOT_ENTRIES) * sizeof(fat_record_t)) + (FAT_SECTOR_SIZE - 1)) / FAT_SECTOR_SIZE;
        uint32_t fatSize = (uint32_t)FAT_GET_16(block, FAT_VOLUME_ID_FAT_SZ_16) ?: FAT_GET_32(block, FAT_VOLUME_ID_FAT_SZ_32);
        uint32_t dataSector = reserved + (fatSize << 1) + rootDirSectors;

        to->sectorsPerCluster = block[FAT_VOLUME_ID_SEC_PER_CLSTR];
        to->fatSector = to->volumeSector + reserved;
        to->dataSector = to->volumeSector + dataSector;
        to->rootCluster = FAT_GET_32(block, FAT_VOLUME_ID_ROOT_CLSTR);

        // now calculate what FS this is
        uint32_t sectors = (uint32_t)FAT_GET_16(block, FAT_VOLUME_ID_SECTORS_16) ?: FAT_GET_32(block, FAT_VOLUME_ID_SECTORS_32);
        uint32_t clusterCount = (sectors - dataSector) / to->sectorsPerCluster;

        if (clusterCount < 4085) {
            // FAT12 which we don't support!
        } else if (clusterCount < 65525) {
            to->type = FS_FAT16;
        } else {
            to->type = FS_FAT32;
        }
    }

    return FAT_NOERR;
}
