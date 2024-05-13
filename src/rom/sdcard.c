#include <stdint.h>
#include <time.h>

#include "serial.h"
#include "sdcard.h"
#include "spi.h"

uint8_t sdcard_wait_response(void);
void sdcard_receive_r7(sdcard_response_t *response);

// com
const sdcard_command_t CMD0 =  {0x40, {0x00, 0x00, 0x00, 0x00}, 0x95};
const sdcard_command_t CMD8 =  {0x48, {0x00, 0x00, 0x01, 0xAA}, 0x87};
const sdcard_command_t CMD16 = {0x50, {0x00, 0x00, 0x02, 0x00}, 0x00};  // configured for 512 byte block size
const sdcard_command_t CMD55 = {0x77, {0x00, 0x00, 0x00, 0x00}, 0x00};
const sdcard_command_t CMD58 = {0x7A, {0x00, 0x00, 0x00, 0x00}, 0x00};

// app commands
const sdcard_command_t CMD41 = {0x69, {0x40, 0x00, 0x00, 0x00}, 0x00};

sdcard_error_t sdcard_init(sdcard_device_t *device) {
    device->status = SDCARD_STATUS_UNKNOWN;
    device->type = SDCARD_DEVICE_NONE;

    spi_init();

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

    for (int tries = 0x10; response.r1 == SDCARD_ERROR_IDLE && tries >= 0; --tries) {
        sdcard_send_app_command(&CMD41, &response);
        switch (response.r1) {
            case SDCARD_NOERR:
                break;
            case SDCARD_ERROR_IDLE:
                delay(10);
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
    int tries = 8;
    uint8_t retval;
    while (--tries >= 0 && (retval = spi_read()) == 0xFF);
    return retval;
}

void sdcard_receive_r7(sdcard_response_t *response) {
    for (int i = 0; i < 4; i++) {
        response->r7[i] = spi_read();
    }
}
