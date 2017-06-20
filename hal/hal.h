#ifndef HAL_HAL_H
#define HAL_HAL_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- 関数定義 -----*/
void hal_init(void);
void __attribute__((__cdecl__)) interrupt_done(u_int inter_n);
void __attribute__((__cdecl__)) enable_interrupt(void);
void __attribute__((__cdecl__)) disable_interrupt(void);
inline void enter_interrupt(void);
inline void exit_interrupt(void);

#endif
