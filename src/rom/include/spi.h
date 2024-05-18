#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SPI_CS  (1<<5)

extern volatile uint8_t MFP_GPIP, MFP_DDR;

#define spi_port    (&MFP_GPIP)
#define spi_ddr     (&MFP_DDR)

// initialize the MFP for SPI
extern void spi_init(void);

// send a value to and receive a value from the connected SPI device
extern int spi_transfer(int);

static inline int spi_read(void) {
    return spi_transfer(0xFF);
}

static inline void spi_cs_assert(void) {
    // *spi_port &= ~SPI_CS;
    __asm__ __volatile__ (
    "   andi.b  %[mask],%[spi]      \n"
    :   [spi] "=m" (MFP_GPIP)
    :   [mask] "n" (~SPI_CS)
    : // clobbers (none)
    );
}

static inline void spi_cs_deassert(void) {
    *spi_port |= SPI_CS;
    // __asm__ __volatile__ (
    // "   ori.b    %[mask],%[spi]      \n"
    // : // no outputs
    // :   [mask] "n" (SPI_CS),
    //     [spi] "=m" (MFP_GPIP)
    // : // clobbers (none)
    //     memory
    // );
}
