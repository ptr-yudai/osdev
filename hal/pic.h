#ifndef HAL_PIC_H
#define HAL_PIC_H

#include "../include/types.h"

/*----- 定数定義 -----*/
// 8259Aマスター
#define	PORT_MASTER_PIC_COMMAND	0x0020
#define	PORT_MASTER_PIC_STATUS  0x0020
#define	PORT_MASTER_PIC_DATA    0x0021
#define	PORT_MASTER_PIC_IMR     0x0021
// 8259Aスレーブ
#define	PORT_SLAVE_PIC_COMMAND  0x00A0
#define	PORT_SLAVE_PIC_STATUS   0x00A0
#define	PORT_SLAVE_PIC_DATA     0x00A1
#define	PORT_SLAVE_PIC_IMR      0x00A1
// ICW1
#define	PIC_ICW1        0x11
// ICW2
#define	PIC_MASTER_ICW2 0x20
#define	PIC_SLAVE_ICW2  0x28
// ICW3
#define	PIC_MASTER_ICW3 0x04
#define	PIC_SLAVE_ICW3  0x02
// ICW4
#define	PIC_MASTER_ICW4 0x01
#define	PIC_SLAVE_ICW4  0x01
// 割り込みマスクレジスタ
#define	PIC_IMR_MASK_IRQ0    0x01
#define	PIC_IMR_MASK_IRQ1    0x02
#define	PIC_IMR_MASK_IRQ2    0x04
#define	PIC_IMR_MASK_IRQ3    0x08
#define	PIC_IMR_MASK_IRQ4    0x10
#define	PIC_IMR_MASK_IRQ5    0x20
#define	PIC_IMR_MASK_IRQ6    0x40
#define	PIC_IMR_MASK_IRQ7    0x80
#define	PIC_IMR_MASK_IRQ_ALL 0xFF
// OCW2のビットマスク
#define	PIC_OCW2_MASK_L1     1
#define	PIC_OCW2_MASK_L2     2
#define	PIC_OCW2_MASK_L3     4
#define	PIC_OCW2_MASK_EOI    0x20
#define	PIC_OCW2_MASK_SL     0x40
#define	PIC_OCW2_MASK_ROTATE 0x80
// OCW3のビットマスク
#define	PIC_OCW3_MASK_RIS  1
#define	PIC_OCW3_MASK_RIR  2
#define	PIC_OCW3_MASK_MODE 4
#define	PIC_OCW3_MASK_SMM  0x20
#define	PIC_OCW3_MASK_ESMM 0x40
#define	PIC_OCW3_MASK_D7   0x80
// コマンド送信
#define PIC1_REG_COMMAND 0x20
#define PIC2_REG_COMMAND 0xA0
// データ送受信
#define PIC1_REG_DATA 0x21
#define PIC2_REG_DATA 0xA1

/*----- 関数定義 -----*/
void pic_sendcmd(u_char cmd, u_char picn);
void pic_senddata(u_char data, u_char picn);
void outb(u_short port, u_char data);
u_char inb(u_short port);
void pic_init();

#endif
