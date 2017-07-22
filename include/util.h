#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#include "types.h"

/*----- 外部関数 -----*/
void __do_div64(void);

/*----- 関数定義 -----*/
u_int strlen(const char*);
void *malloc(u_int);
void free(void* addr, u_int size);
void *memcpy(void*, const void*, u_int);
void *memset(void*, u_char, u_int);
u_int isascii(u_int c);
void itoa(int value, char *str, int base);
void unicode2ascii(char* str, u_int len);
u_int64 do_div64(u_int64, u_int);
u_int do_mod64(u_int64, u_int);
void exit(void);

#endif
