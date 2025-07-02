#pragma once

#include "stdint.h"

typedef struct {
    char ident[4];
    uint32_t eof;
    uint32_t version;
    uint32_t sn76489;
    uint32_t ym2413;
    uint32_t gd3;
    uint32_t samples;
    uint32_t loop_offset;
    uint32_t loop_samples;
    uint32_t rate;
    uint16_t sn_fb;
    uint8_t sn_srw;
    uint8_t sn_flags;
    uint32_t ym2612;
    uint32_t ym2151;
    uint32_t data;
    uint32_t sega_pcm;
    uint32_t spcm_interface;
    uint32_t rf5c68;
    uint32_t ym2203;
    uint32_t ym2608;
    uint32_t ym2610;
    uint32_t ym3812;
    uint32_t ym3526;
    uint32_t y8950;
    uint32_t ymf262;

} __attribute__((packed)) vgm_header_t;

void vgm_play(void);
