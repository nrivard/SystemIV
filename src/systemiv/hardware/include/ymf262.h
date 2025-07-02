#pragma once

#include <stdint.h>

#define ymf262_index        ((volatile uint8_t * const)(0x800001))
#define ymf262_data         ((volatile uint8_t * const)(0x800003))

typedef enum {
    OPL_ERROR_NONE,
    OPL_ERROR_NOT_FOUND
} opl_error_t;

typedef enum {
    YMF262_OPL2 = 2,
    YMF262_OPL3
} opl_device_type_t;

typedef struct {
    opl_device_type_t type;
    uint32_t clock;
} opl_device_t;

// initialize OPL3
opl_error_t opl_init(opl_device_t *device);

void ymf262_play_test(void);

static inline void opl_write(uint16_t reg, uint8_t data) {
    // TODO: do work for table 2 registers!
    *ymf262_index = reg;

    asm volatile(
        "nop\n" \
        "nop"
        :
        :
    );

    *ymf262_data = data;

    asm volatile(
        "nop\n" \
        "nop"
        :
        :
    );
}

static inline unsigned char opl_read() {
    return *ymf262_index;
}
