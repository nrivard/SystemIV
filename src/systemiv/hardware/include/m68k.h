#pragma once

#include <proc.h>
#include <stdint.h>

#define     MK68K_IRQ_MASK      0x0700

typedef struct {
    void *sp;
    void *reset;
    void *bus_error;
    void *addr_error;
    void *illegal_instr;
    void *div_by_zero;
    void *chk_instr;
    void *trapv;
    void *priv_viol;
    void *trace;
    void *a_line;
    void *f_line;
    void *res_1[3];
    void *uninit;
    void *res_2[8];
    void *spurious;
    void *auto_vec[7];
    void *trap[16];
    void *res_3[16];
    void *user[0xC0];
} m68k_vector_table_t;

static m68k_vector_table_t * const m68k_vector_table = (m68k_vector_table_t *)((void *)0x00000000);

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
        "move.w     %0,%%sr" 
        : // no output
        : "d" (sr)
    );
}

static inline void m68k_save_ctx(proc_ctx_t *ctx) {
    asm volatile (
        "movem.l    %%d2-%%d7/%%a2-%%usp, %0\n\t" \
        "move.l     %%usp, %1"
        : "=m" (*ctx), "=m" (ctx->sp)
        :
        :
    );
}

static inline void m68k_load_ctx(proc_ctx_t *ctx) {
    asm volatile (
        "move.l     %0, %%usp\n\t" \
        "movem.l    %1, %%d2-%%d7/%%a2-%%a6"
        : "=m" (ctx->sp), "=m" (*ctx)
        :
        :
    );
}
