#pragma once

void *memcpy(void *dest, const void *src, unsigned int n);
void *memset(void *dest, int c, unsigned int n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned int n);
int strlcpy(char *dest, const char *src, unsigned int n);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, unsigned int n);
