#ifndef HAL_IRQ_H
#define HAL_IRQ_H

#include "../include/types.h"

/*----- IRQのインポート -----*/
void irq_pit_bridge(void);
void irq_keyboard_bridge(void);

/*----- 割り込みハンドラ定義 -----*/
void irq_pit(void);
void irq_keyboard(void);

#endif
