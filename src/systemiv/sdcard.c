#include <stdint.h>
// #include <time.h>

#include "sdcard.h"
#include "spi.h"

#define SDCARD_OCR_CCS      0b01000000
#define SDCARD_MAX_BUSY_ATTEMPTS  0x1000

void sdcard_send_command(const sdcard_command_t *const command, 
                         sdcard_response_t *response);

void sdcard_send_app_command(const sdcard_command_t *const command,
                             sdcard_response_t *response);

uint8_t sdcard_wait_response(void);
uint8_t sdcard_wait_busy(void);
void sdcard_receive_r7(sdcard_response_t *response);


// com
static const sdcard_command_t CMD0 =  {0x40, 0x00000000, 0x95};
static const sdcard_command_t CMD8 =  {0x48, 0x000001AA, 0x87};
static const sdcard_command_t CMD16 = {0x50, 0x00000200, 0x00};  // configured for 512 byte block size
static const sdcard_command_t CMD55 = {0x77, 0x00000000, 0x00};
static const sdcard_command_t CMD58 = {0x7A, 0x00000000, 0x00};

// app commands
static const sdcard_command_t CMD41 = {0x69, 0x40000000, 0x00};

static const char* sdcard_device_type_msgs[] = {
    "Unknown",
    "SD card V1",
    "SD card V2 SC",
    "SD card V2 XC"
};

sdcard_error_t sdcard_init(sdcard_device_t *device) {
    device->status = SDCARD_STATUS_UNKNOWN;
    device->type = SDCARD_DEVICE_NONE;

    spi_init();

    // toggle ENABLE to sync card timing up
    spi_cs_assert();
    spi_cs_deassert();

    // get sd card into spi mode with CS de-asserted
    for (int i = 0; i < 10; i++) {
        spi_read();
    }

    spi_cs_assert();

    sdcard_response_t response;
    sdcard_send_command(&CMD0, &response);
    if (response.r1 != SDCARD_ERROR_IDLE) {
        goto ERROR;
    }

    sdcard_send_command(&CMD8, &response);
    if (response.r1 == SDCARD_ERROR_ILLEGAL) {
        // v1 SD card
        device->type = SDCARD_DEVICE_V1;

        // set to idle for next phase
        response.r1 = SDCARD_ERROR_IDLE;
    } else if (response.r1 == SDCARD_ERROR_IDLE) {
        // check v2 SD card
        sdcard_receive_r7(&response);

        if (!(
            response.r7[0] == 0 &&
            response.r7[1] == 0 &&
            response.r7[2] == 1 &&
            response.r7[3] == 0xAA
        )) {
            response.r1 = SDCARD_ERROR_ILLEGAL;
            goto ERROR;
        }

        // assume SC until we check later
        device->type = SDCARD_DEVICE_V2_SC;
    } else {
        goto ERROR;
    }

    for (int tries = 0x20; response.r1 == SDCARD_ERROR_IDLE && tries >= 0; --tries) {
        sdcard_send_app_command(&CMD41, &response);
        switch (response.r1) {
            case SDCARD_NOERR:
                break;
            case SDCARD_ERROR_IDLE:
                // TODO: restore!
                // delay(5);
                continue;
            default:
                goto ERROR;
        }
    }

    // get OCR to see if XC or SC
    if (device->type > SDCARD_DEVICE_V1) {
        sdcard_send_command(&CMD58, &response);
        if (response.r1 != SDCARD_NOERR) {
            goto ERROR;
        }
        sdcard_receive_r7(&response);
        device->type = response.r7[0] & SDCARD_OCR_CCS ? SDCARD_DEVICE_V2_XC : SDCARD_DEVICE_V2_SC;
    }

    // if v1 or SC, set to 512 byte blocks
    if (device->type < SDCARD_DEVICE_V2_XC) {
        sdcard_send_command(&CMD16, &response);
        if (response.r1 != SDCARD_NOERR) {
            goto ERROR;
        }
    }

    // turn on fastest speed
    spi_set_speed(spi_speed_fastest);

    // initialized and device type is set. response.r1 is already NOERR
    device->status = SDCARD_STATUS_READY;
    goto DONE;

ERROR:
    device->status = SDCARD_STATUS_FAILED;
    device->type = SDCARD_DEVICE_NONE;

DONE:
    spi_cs_deassert();
    return response.r1;
}

sdcard_error_t sdcard_read_block(uint32_t block, uint8_t buffer[512], sdcard_data_token_t *token) {
    *token = SDCARD_DATA_TOKEN_NONE;

    sdcard_command_t read;
    read.index = 0x51;
    read.arg = block;
    read.crc = 0x00;

    spi_cs_assert();

    sdcard_response_t response;
    sdcard_send_command(&read, &response);   
    if (response.r1 != SDCARD_NOERR) {
        goto DONE;
    }

    *token = sdcard_wait_response();
    if (*token != SDCARD_DATA_TOKEN_BLOCK) {
        response.r1 |= SDCARD_ERROR_FOUND;
        goto DONE;
    }

    for (int i = 0; i < 512; i++) {
        *buffer++ = spi_read();
    }

    // "read" 16 bit CRC
    spi_read();
    spi_read();

DONE:
    spi_cs_deassert();
    return response.r1;
}

sdcard_error_t sdcard_read_block_n(uint32_t start, uint32_t count, uint8_t *buffer, sdcard_data_token_t *token) {
    *token = SDCARD_DATA_TOKEN_NONE;

    sdcard_command_t multiread;
    multiread.index = 0x52;
    multiread.arg = start;
    multiread.crc = 0x00;

    spi_cs_assert();

    sdcard_response_t response;
    sdcard_send_command(&multiread, &response);
    if (response.r1 != SDCARD_NOERR) {
        goto DONE;
    }

    for (int i = 0; i < count; i++) {
        // need to wait for data start token for each block
        *token = sdcard_wait_response();
        if (*token != SDCARD_DATA_TOKEN_BLOCK) {
            response.r1 |= SDCARD_ERROR_FOUND;
            goto DONE;
        }

        for (int j = 0; j < 512; j++) {
            *buffer++ = spi_read();
        }

        // read 16 bit CRC for this block
        spi_read();
        spi_read();
    }

    sdcard_command_t stop;
    stop.index = 0x4C;
    stop.arg = 0x00;
    stop.crc = 0x00;

    sdcard_send_command(&stop, &response);
    if (response.r1 != SDCARD_NOERR) {
        goto DONE;
    }
    
    *token = sdcard_wait_response();

DONE:
    spi_cs_deassert();
    return response.r1;
}

sdcard_error_t sdcard_write_block(uint32_t block, uint8_t const buffer[512], sdcard_data_token_t *token) {
    *token = SDCARD_DATA_TOKEN_NONE;

    sdcard_command_t write;
    write.index = 0x58;
    write.arg = block;
    write.crc = 0x00;

    spi_cs_assert();

    sdcard_response_t response;
    sdcard_send_command(&write, &response);   
    if (response.r1 != SDCARD_NOERR) {
        goto DONE;
    }

    spi_transfer(SDCARD_DATA_TOKEN_BLOCK);

    for (int i = 0; i < 512; i++) {
        spi_transfer(*buffer++);
    }

    // send dummy CRC
    spi_transfer(0x00);
    spi_transfer(0x00);

    *token = sdcard_wait_response();
    if ((*token & 0x1F) != SDCARD_DATA_RESPONSE_ACCEPTED) {
        response.r1 |= SDCARD_ERROR_FOUND;
        goto DONE;
    }

    *token = sdcard_wait_busy();

DONE:
    spi_cs_deassert();
    return response.r1;
}

sdcard_error_t sdcard_write_block_n(uint32_t start, uint32_t count, uint8_t const * buffer, sdcard_data_token_t *token) {
    *token = SDCARD_DATA_TOKEN_NONE;

    sdcard_command_t multiwrite;
    multiwrite.index = 0x59;
    multiwrite.arg = start;
    multiwrite.crc = 0x00;

    spi_cs_assert();

    sdcard_response_t response;
    sdcard_send_command(&multiwrite, &response);
    if (response.r1 != SDCARD_NOERR) {
        goto DONE;
    }

    for (int i = 0; i < count; i++) {
        // send data start token for each block
        spi_transfer(SDCARD_DATA_TOKEN_MULTI);

        for (int j = 0; j < 512; j++) {
            spi_transfer(*buffer++);
        }

        // send 16 bit
        spi_transfer(0x00);
        spi_transfer(0x00);

        *token = sdcard_wait_response();
        if ((*token & 0x1F) != SDCARD_DATA_RESPONSE_ACCEPTED) {
            response.r1 |= SDCARD_ERROR_FOUND;
            goto DONE;
        }

        *token = sdcard_wait_busy();
    }

    spi_transfer(SDCARD_DATA_TOKEN_STOP);
    *token = sdcard_wait_busy();

DONE:
    spi_cs_deassert();
    return response.r1;
}

void sdcard_send_command(const sdcard_command_t *const command, sdcard_response_t *response) {
    // always send a dummy byte
    spi_read();

    uint8_t *bytes = (uint8_t *)command;
    for (int i = 0; i < sizeof(sdcard_command_t); i++) {
        spi_transfer(bytes[i]);
    }

    response->r1 = sdcard_wait_response();
}

void sdcard_send_app_command(const sdcard_command_t *const command, sdcard_response_t *response) {
    sdcard_send_command(&CMD55, response);
    if (response->r1 > SDCARD_ERROR_IDLE) {
        return;
    }

    sdcard_send_command(command, response);
}

uint8_t sdcard_wait_response() {
    // NCR is 0 to 8 bytes of wait time, beyond that is out of spec
    int tries = 0x1000;
    uint8_t retval;
    while (--tries >= 0 && (retval = spi_read()) == 0xFF);
    return retval;
}

uint8_t sdcard_wait_busy() {
    int tries = SDCARD_MAX_BUSY_ATTEMPTS;
    uint8_t retval;
    while (--tries >= 0 && (retval = spi_read()) == 0x00);
    return retval;
}

void sdcard_receive_r7(sdcard_response_t *response) {
    for (int i = 0; i < 4; i++) {
        response->r7[i] = spi_read();
    }
}

const char *sdcard_device_type_msg(sdcard_device_type_t type) {
    return sdcard_device_type_msgs[type];
}
