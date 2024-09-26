#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SPI_CS  (1<<5)

extern void SPIASSERT();
extern void SPIDEASSERT();

#define spi_cs_assert()     SPIASSERT()
#define spi_cs_deassert()   SPIDEASSERT()

// initialize the MFP for SPI
extern void spi_init(void);

// send a value to and receive a value from the connected SPI device
extern int spi_transfer(int);

static inline int spi_read(void) {
    return spi_transfer(0xFF);
}

// KEEP as an example of (previously) working inline assembly

// static inline void spi_cs_assert(void) {
//     SPIASSERT();
//     // *spi_port &= ~SPI_CS;
//     // __asm__ __volatile__ (
//     // "   andi.b  %[mask],%[spi]      \n"
//     // :   [spi] "=m" (MFP_GPIP)
//     // :   [mask] "n" (~SPI_CS)
//     // : // clobbers (none)
//     // );
// }

// static inline void spi_cs_deassert(void) {
//     SPIDEASSERT();
//     // *spi_port |= SPI_CS;
//     // __asm__ __volatile__ (
//     // "   ori.b    %[mask],%[spi]      \n"
//     // :   [spi] "=m" (MFP_GPIP)
//     // :   [mask] "n" (SPI_CS)
//     // : // clobbers (none)
//     // );
// }
