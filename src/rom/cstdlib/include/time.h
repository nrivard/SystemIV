#pragma once

#include <stdint.h>

// this is really the number of `ticks` encountered as time precision ain't so great here
#define CLOCKS_PER_SEC  100

typedef long int clock_t;

// returns system uptime
clock_t clock();
