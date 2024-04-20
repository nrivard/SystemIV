#include "time.h"
#include <stdint.h>

extern uint8_t  SystemJiffies;
extern uint32_t SystemUptime;

clock_t clock() {
    // jiffies count down so subtract current value from 100
    return (SystemUptime * CLOCKS_PER_SEC) + (CLOCKS_PER_SEC - (uint32_t)SystemJiffies);
}
