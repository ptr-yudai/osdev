#ifndef HAL_PIT_H
#define HAL_PIT_H

#include "../include/types.h"

/*----- 定数 -----*/
// 操作コマンドのビットマスク
#define	PIT_OCW_MASK_BINCOUNT 1
#define	PIT_OCW_MASK_MODE     0xE
#define	PIT_OCW_MASK_RL	      0x30
#define	PIT_OCW_MASK_COUNTER  0xC0
// カウンタ
#define PIT_REG_COUNTER0 0x40
#define PIT_REG_COUNTER1 0x41
#define PIT_REG_COUNTER2 0x42
#define PIT_REG_COMMAND  0x43
#define PIT_OCW_COUNTER0 0
#define PIT_OCW_COUNTER1 0x40
#define PIT_OCW_COUNTER2 0x80
// モード
#define	PIT_OCW_MODE_TERMINALCOUNT 0x0
#define	PIT_OCW_MODE_ONESHOT       0x2
#define	PIT_OCW_MODE_RATEGEN       0x4
#define	PIT_OCW_MODE_SQUAREWAVEGEN 0x6
#define	PIT_OCW_MODE_SOFTWARETRIG  0x8
#define	PIT_OCW_MODE_HARDWARETRIG  0xA
// データ
#define	PIT_OCW_RL_LATCH   0x00
#define	PIT_OCW_RL_LSBONLY 0x10
#define	PIT_OCW_RL_MSBONLY 0x20
#define	PIT_OCW_RL_DATA	   0x30

/*----- 関数定義 -----*/
void pit_init(void);
void pit_increment_tick(void);
u_int pit_settick(u_int t);
u_int pit_gettick(void);
void pit_sendcmd(u_char cmd);
void pit_senddata(u_short data, u_char counter);
u_char pit_readdata(u_char counter);
void pit_start_counter(u_int freq, u_char counter, u_char mode);

#endif
