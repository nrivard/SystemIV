#ifdef SPIGATE

#include "spigate.h"

void spigate_init() {
    *spigate_cmd = 0x03; // everything off, lowest speed setting
}

int spigate_transfer(int data) {
    *spigate_data = data;
    while (!(*spigate_cmd & SPIGATE_CMD_ITC));
    return *spigate_data;
}

void spigate_set_speed(spigate_speed_t speed) {
    uint8_t cmd = *spigate_cmd;
    cmd &= ~SPIGATE_CMD_CLK_SEL;
    cmd |= speed & SPIGATE_CMD_CLK_SEL;
    *spigate_cmd = cmd;
}

void spigate_assert() {
    uint8_t cmd = *spigate_cmd;
    cmd |= SPIGATE_CMD_DEN;
    *spigate_cmd = cmd;
}

void spigate_deassert() {
    uint8_t cmd = *spigate_cmd;
    cmd &= ~SPIGATE_CMD_DEN;
    *spigate_cmd = cmd;
}

#endif
