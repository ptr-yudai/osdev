#ifndef HAL_HAL_H
#define HAL_HAL_H

#include "../include/types.h"

/*----- 関数定義 -----*/
void hal_init(void);
void __attribute__((__cdecl__)) interrupt_done(u_int inter_n);
void __attribute__((__cdecl__)) enable_interrupt(void);
void __attribute__((__cdecl__)) disable_interrupt(void);
void enter_interrupt(void);
void exit_interrupt(void);

#endif
