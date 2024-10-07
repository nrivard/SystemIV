#pragma once

#include <stdint.h>

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
    SDCARD_ERROR_PARAM =        (1<<6),
    SDCARD_ERROR_FOUND =        (1<<7),
} sdcard_error_t;

typedef enum {
    SDCARD_DEVICE_NONE = 0,
    SDCARD_DEVICE_V1,
    SDCARD_DEVICE_V2_SC,    // standard capacity v2 SD card
    SDCARD_DEVICE_V2_XC     // high/extended capacity v2 SD card
} sdcard_device_type;

typedef enum {
    SDCARD_DATA_TOKEN_NONE  = 0xFF,
    SDCARD_DATA_TOKEN_BLOCK = 0xFE,
    SDCARD_DATA_TOKEN_MULTI = 0xFC,
    SDCARD_DATA_TOKEN_STOP  = 0xFD,
    SDCARD_DATA_TOKEN_ERR   = (1<<0),
    SDCARD_DATA_TOKEN_CC_ERR = (1<<1),
    SDCARD_DATA_TOKEN_ECC_FAIL = (1<<2),
    SDCARD_DATA_TOKEN_RANGE_ERR = (1<<3)
} sdcard_data_token_t;

typedef enum {
    SDCARD_DATA_RESPONSE_ACCEPTED   = 0x05,
    SDCARD_DATA_RESPONSE_ERR_CRC    = 0x0B,
    SDCARD_DATA_RESPONSE_ERR_WRITE  = 0x0D
} sdcard_data_response_t;

typedef struct {
    uint8_t index;
    uint32_t arg;
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

/// @brief 
/// @param block    block number to read from
/// @param buffer   512 byte buffer to copy data to
/// @param token 
/// @return error code. will contain `SDCARD_ERROR_FOUND` if error was found. read `token` for possible error reason
sdcard_error_t sdcard_read_block(uint32_t block, uint8_t buffer[512], sdcard_data_token_t *token);

/// @brief 
/// @param start    starting block number to read from
/// @param count    how many contiguous blocks to read
/// @param buffer   a buffer that is at least `512 * count` bytes long. It is up to you that this is safe!
/// @param token 
/// @return error code. will contain `SDCARD_ERROR_FOUND` if error was found. read `token` for possible error reason
sdcard_error_t sdcard_read_block_n(uint32_t start, uint32_t count, uint8_t *buffer, sdcard_data_token_t *token);

/// @brief 
/// @param block    block number to write to
/// @param buffer   block of 512 bytes to write. ALL 512 will get written!
/// @param token    
/// @return error code. will contain `SDCARD_ERROR_FOUND` if error was found. read `token` for possible error reason
sdcard_error_t sdcard_write_block(uint32_t block, uint8_t const buffer[512], sdcard_data_token_t *token);

/// @brief 
/// @param start    starting block number to write to
/// @param count    how many contiguous blocks to write
/// @param buffer   a buffer that is at least `512 * count` bytes long. It is up to you that this is safe!
/// @param token 
/// @return error code. will contain `SDCARD_ERROR_FOUND` if error was found. read `token` for possible error reason
sdcard_error_t sdcard_read_block_n(uint32_t start, uint32_t count, uint8_t * const buffer, sdcard_data_token_t *token);
