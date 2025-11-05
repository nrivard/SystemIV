#pragma once

typedef enum {
    NOERR = 0,
    ERR_NULL_PTR,
    ERR_IDX_OUT_OF_BOUNDS,

    // device errors
    ERR_DEV_BLK_SZ = 0x1000,
    ERR_DEV_NOT_FOUND,
    ERR_DEV_FULL,
} oserr_t;
