#pragma once

#include <stdint.h>

typedef enum {
    XMODEM_NOERR = 0,
    XMODEM_ERROR_TIMEOUT,
} xmodem_error_t;

uint8_t xmodem_recv(uint8_t *destination, int maxsize);
