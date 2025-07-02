#pragma once

/* Offset of member MEMBER in a struct of type TYPE. */
#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)

#define swap_endian32(num)  __builtin_bswap32(num)
#define swap_endian16(num)  __builtin_bswap16(num)
