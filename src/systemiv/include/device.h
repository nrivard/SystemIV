#pragma once

#include <oserr.h>

typedef struct {
    // block size. Character devices should set this to 1.
    // device is unitialized if blk_size is `0`
    unsigned int blk_size;
    
    // reads a block of `blk_size`. Device interprets `ctx` as it will
    oserr_t (*read_blk)(unsigned int blk, char *buffer, void *ctx);

    // reads `n` number of blocks consecutively of `blk_size`, starting at `blk`. Device interprets `ctx` as it will
    oserr_t (*read_blk_n)(unsigned int blk, unsigned int n, char *const buffer, void *ctx);

    // writes a block of `blk_size`. Device interprets `ctx` as it will
    oserr_t (*write_blk)(unsigned int blk, char const *buffer, void *ctx);

    // writes `n` number of blocks consecutively of `blk_size`, starting at `blk`. Device interprets `ctx` as it will
    oserr_t (*write_blk_n)(unsigned int blk, unsigned int n, char const *buffer, void *ctx);

    // TODO: do we want things like user-facing description of the device? name? etc.
} device_t;

oserr_t device_add(device_t *device, unsigned int *idx);
oserr_t device_remove(unsigned int idx);
oserr_t device_get(unsigned int idx, device_t *device);
