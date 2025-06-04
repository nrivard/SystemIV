#pragma once

void *memcpy(void *dest, const void *src, int n);
void *memset(void *dest, int c, int n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int n);
int strlcpy(char *dest, const char *src, int n);