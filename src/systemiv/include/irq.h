#include "m68k.h"

static inline void irq_on() {
    m68k_wr_sr(m68k_rd_sr() & ~MK68K_IRQ_MASK);
}

static inline void irq_off() {
    m68k_wr_sr(m68k_rd_sr() | MK68K_IRQ_MASK);
}
