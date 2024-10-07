#pragma once

#include <stdint.h>

static inline uint16_t m68k_rd_sr() {
    uint16_t x;
    asm volatile (
        "move.w   %%sr,%0" 
        : "=d" (x)
        :
    );
    return x;
}

static inline void m68k_wr_sr(uint16_t sr) {
    asm volatile (
        "move.w   %0,%%sr" 
        : // no output
        : "d" (sr)
    );
}
