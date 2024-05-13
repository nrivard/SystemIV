#pragma once

#include <stdint.h>

#define SDCARD_OCR_CCS      0b01000000

typedef enum {
    SDCARD_STATUS_UNKNOWN = 0,
    SDCARD_STATUS_READY,
    SDCARD_STATUS_FAILED
} sdcard_status_t;

typedef enum {
    SDCARD_NOERR =              0,
    SDCARD_ERROR_IDLE =         (1<<0),
    SDCARD_ERROR_ERASE_RESET =  (1<<1),
    SDCARD_ERROR_ILLEGAL =      (1<<2),
    SDCARD_ERROR_CRC =          (1<<3),
    SDCARD_ERROR_SEQ =          (1<<4),
    SDCARD_ERROR_ADDR =         (1<<5),
    SDCARD_ERROR_PARAM =        (1<<6)
} sdcard_error_t;

typedef enum {
    SDCARD_DEVICE_NONE = 0,
    SDCARD_DEVICE_V1,
    SDCARD_DEVICE_V2_SC,    // standard capacity v2 SD card
    SDCARD_DEVICE_V2_XC     // high/extended capacity v2 SD card
} sdcard_device_type;

typedef struct {
    uint8_t index;
    uint8_t args[4];
    uint8_t crc;
} __attribute__((packed)) sdcard_command_t;

typedef struct {
    uint8_t data[512];
    uint16_t crc;
    uint8_t token;
} __attribute__((packed)) sdcard_block_t;

typedef struct {
    sdcard_error_t r1;
    uint8_t r7[4];
} __attribute__((packed)) sdcard_response_t;

typedef struct {
    sdcard_status_t status;
    sdcard_device_type type;
} __attribute__((packed)) sdcard_device_t;

sdcard_error_t sdcard_init(sdcard_device_t *device);

void sdcard_send_command(const sdcard_command_t *const command, 
                         sdcard_response_t *response);

void sdcard_send_app_command(const sdcard_command_t *const command,
                             sdcard_response_t *response);
