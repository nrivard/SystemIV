#pragma once

typedef enum {
    NOERR = 0,
    ERR_NULL_PTR,

    // device errors
    ERR_DEV_BLK_SZ = 0x1000,
} oserr_t;
