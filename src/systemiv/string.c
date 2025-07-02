#include "string.h"

#include "ctype.h"

void *memcpy(void *dest, const void *src, unsigned int n) {
    char *to = (char *)dest;
    char *from = (char *)src;
    for (char *end = from + n; from < end; *to++ = *from++);

    return to;
}

void *memset(void *dest, int c, unsigned int n) {
    char *to = (char *)dest;
    for (char *end = to + n; to < end; *to++ = c);

    return to;
}

char *strcpy(char *dest, const char *src) {
    char *to = dest;
    while ((*to++ = *src++));
    return dest;
}

char *strncpy(char *dest, const char *src, unsigned int n) {
    int i = 0;

    for ( ; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    for ( ; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
}

int strlcpy(char *dest, const char *src, unsigned int n) {
    int i = 0;

    for ( ; i < n - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    dest[i] = '\0';

    return i;
}

int strcmp(const char *str1, const char *str2) {
    register const char *left = str1, *right = str2;
    register char result;
    for (;;) {
        if (!*left) {
            return *right ? -1 : 0;
        } else if (!*right) {
            return 1;
        } else if ((result = *left++ - *right++) != 0) {
            return (int)result;
        }
    }
    return 0;
}

int strncmp(const char *str1, const char *str2, unsigned int n) {
    register char result;
    for (int i = 0; i < n; i++) {
        if (!str1[i]) {
            return str2[i];
        } else if (!str2[i]) {
            return str1[i];
        } else if ((result = str1[i] - str2[i]) != 0) {
            return (int)result;
        }
    }
    return 0;
}
