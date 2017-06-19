#ifndef HAL_IRQ_H
#define HAL_IRQ_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- IRQのインポート -----*/
void irq_pit_bridge(void);
void irq_keyboard_bridge(void);

/*----- 割り込みハンドラ定義 -----*/
void irq_pit(void);
void irq_keyboard(void);

#endif
