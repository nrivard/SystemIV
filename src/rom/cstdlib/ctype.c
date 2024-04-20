#include "ctype.h"

int isdigit(int c) {
    return (c >= '0' && c <= '9') ? 1 : 0;
}

int isalpha(int c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) ? 1 : 0;
}

int isupper(int c) {
    return (c >= 'A' && c <= 'Z') ? 1 : 0;
}

int islower(int c) {
    return (c >= 'a' && c <= 'z') ? 1 : 0;
}
