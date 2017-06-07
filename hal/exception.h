#ifndef HAL_EXCEPTION_H
#define HAL_EXCEPTION_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- 関数定義 -----*/
void kernel_panic(const char* msg);

/*----- 割り込みハンドラ定義 -----*/
void handler_divided_by_zero();
//void handler_signal_step();
//void handler_nmi();
//void handler_breakpoint();
//void handler_overflow();
//void handler_bounds_check();

#endif