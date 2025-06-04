#include "string.h"

#include "ctype.h"

void *memcpy(void *dest, const void *src, int n) {
    char *to = (char *)dest;
    char *from = (char *)src;
    for (char *end = from + n; from < end; *to++ = *from++);

    return to;
}

void *memset(void *dest, int c, int n) {
    char *to = (char *)dest;
    for (char *end = to + n; to < end; *to++ = c);

    return to;
}

char *strcpy(char *dest, const char *src) {
    char *to = dest;
    while ((*to++ = *src++));
    return dest;
}

char *strncpy(char *dest, const char *src, int n) {
    int i = 0;

    for ( ; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    
    for ( ; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
}

int strlcpy(char *dest, const char *src, int n) {
    int i = 0;

    for ( ; i < n - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    dest[i] = '\0';

    return i;
}
