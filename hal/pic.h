#ifndef HAL_PIC_H
#define HAL_PIC_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- 定数定義 -----*/
// PIC1レジスタポートアドレス
#define I86_PIC1_REG_COMMAND 0x20
#define I86_PIC1_REG_STATUS  0x20
#define I86_PIC1_REG_DATA    0x21
#define I86_PIC1_REG_IMR     0x21
// PIC2レジスタポートアドレス
#define I86_PIC2_REG_COMMAND 0xA0
#define I86_PIC2_REG_STATUS  0xA0
#define I86_PIC2_REG_DATA    0xA1
#define I86_PIC2_REG_IMR     0xA1

/*----- 関数定義 -----*/
void outb(u_short port, u_char data);
u_char inb(u_short port);



#endif
