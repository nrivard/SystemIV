#pragma once

#include <stdint.h>

// this is really the number of `ticks` encountered as time precision ain't so great here
#define CLOCKS_PER_SEC  100

typedef long int clock_t;

// returns system uptime
clock_t clock();

// delay approximately the specified number of clock ticks (see `CLOCKS_PER_SEC` for definition)
// this does not do sub-tick adjustment so is always somewhere between `ticks` and `ticks - 1`!
void delay(int ticks);
