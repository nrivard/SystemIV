#pragma once

#include <stdint.h>

// typedef struct {
//     int (*read)(int file, uint8_t * buffer, int count);
//     int (*write)(int file, uint8_t *buffer, int count);
// } device_t;

typedef enum {
    FILE_TYPE_NONE = 0,
    FILE_TYPE_PIPE,
    FILE_TYPE_INODE,
    FILE_TYPE_DEVICE
} file_type_t;

typedef struct {
    file_type_t type;
    int ref_count;
    char readable;
    char writeable;
} file_t;
