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
    uint16_t sectors16;     // always zero for FAT32
    uint8_t mediaDescriptor;
    uint16_t fatSize16;       // only valid for FAT16!
    uint16_t sectorsPerTrack;
    uint16_t heads;
    uint32_t hiddenSectors;
    uint32_t sectors32;

    union {
        struct {
            uint32_t fatSize32;
            uint16_t flags;
            uint8_t version[2]; // major and minor filesystem version
            uint32_t rootCluster;
            // ignoring remaining fields
        } fat32;
    };
} __attribute((packed)) fat_volume_id_t;

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

// fetches volume ID sector and completes initialization for fat32 volume
fat_error_t fat_init_32(fat_volume_t *volume, uint32_t lba);

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
        fat_mbr_partition_t *from = &partitions[i];
        fat_volume_t *to = &(disk->volumes[i]);

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

        fat_volume_id_t *volumeID = (fat_volume_id_t *)block;

        if (swap_endian16(volumeID->sectorSize) != FAT_SECTOR_SIZE) {
            continue;
        }

        if (volumeID->fats != 2) {
            continue;
        }

        uint16_t reserved = swap_endian16(volumeID->reservedSectors);
        uint32_t rootDirSectors = ((swap_endian16(volumeID->rootEntries) * sizeof(fat_record_t)) + (FAT_SECTOR_SIZE - 1)) / FAT_SECTOR_SIZE;
        uint32_t fatSize = swap_endian32((uint32_t)volumeID->fatSize16 ?: volumeID->fat32.fatSize32);
        uint32_t dataSector = reserved + (fatSize << 1) + rootDirSectors;

        to->sectorsPerCluster = volumeID->sectorsPerCluster;
        to->fatSector = to->volumeSector + reserved;
        to->dataSector = to->volumeSector + reserved + dataSector;
        to->rootCluster = swap_endian32(volumeID->fat32.rootCluster);

        // now calculate what FS this is
        uint32_t sectors = swap_endian32((uint32_t)volumeID->sectors16 ?: volumeID->sectors32);
        uint32_t clusterCount = (sectors - dataSector) / volumeID->sectorsPerCluster;

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
