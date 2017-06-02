#ifndef HAL_PIC_H
#define HAL_PIC_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

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

/*----- 関数定義 -----*/
void outb(u_short port, u_char data);
u_char inb(u_short port);
void pic_init();


#endif
