#ifndef HAL_IRQ_H
#define HAL_IRQ_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- 割り込みハンドラ定義 -----*/
void __attribute__((__cdecl__)) irq_pit(void);
void __attribute__((__cdecl__)) irq_keyboard(void);

#endif
