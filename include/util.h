#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

typedef unsigned int u_int;
typedef unsigned char u_char;

u_int strlen(const char*);
void *memcpy(void*, const void*, u_int);
void *memset(void*, u_char, u_int);

#endif
