#include "time.h"
#include <stdint.h>

extern uint8_t  SystemJiffies;
extern uint32_t SystemUptime;

clock_t clock() {
    return (SystemUptime * CLOCKS_PER_SEC) + (uint32_t)SystemJiffies;
}

void delay(int ticks) {
    clock_t start = clock();
    while ((clock() - start) < ticks);
}
