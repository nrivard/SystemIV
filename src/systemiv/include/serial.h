#pragma once

#include <device.h>
#include <oserr.h>

oserr_t serial_init(device_t *device);

void serial_put(char c);
void serial_put_string(const char *s);
void serial_put_hex(const char c);
void serial_put_long(const unsigned long d);
