#include "string.h"
#include "ctype.h"

int strlen(const char *str) {
    register int len;
    for (len = 0; *str++ != 0; len++);
    return len;
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
}

unsigned int _ascii2byte(unsigned int c) {
    register unsigned int b;

    if ((b = c - '0') < 0xA) {
        return b;
    }

    b -= 0x7;   // 'A' - '9' - 1;
    if (b < 0x10) {
        return b;
    }

    return b - 0x20;    // 'a' - 'A';
}

// TODO: doesn't eat white space yet!
// TODO: doesn't support base zero yet!
unsigned long strtoul(const char *str, char **endptr, register int base) {
    register const char *ptr = str;
    register unsigned int c = *ptr++;
    register unsigned long result;

    for (result = 0; ;c = *ptr++) {
        c = _ascii2byte(c);
        if (c >= base) {
            break;
        }
        result = (result * base) + c;
    }

    if (endptr) {
        *endptr = (char *)ptr;
    }

    return result;
}
