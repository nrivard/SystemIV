#pragma once

#include <stdint.h>

typedef struct {
    uint8_t index;
    uint8_t args[4];
    uint8_t crc;
} __attribute__((packed)) sdcard_command_t;
