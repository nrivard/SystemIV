#pragma once

void *memcpy(void *dest, const void *src, int n);
void *memset(void *dest, int c, int n);
int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
unsigned long strtoul(const char *str, char **endptr, int base);
