#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#include "types.h"

u_int strlen(const char*);
void *memcpy(void*, const void*, u_int);
void *memset(void*, u_char, u_int);
u_int isascii(u_int c);

#endif
