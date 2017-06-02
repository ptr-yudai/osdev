#include "pic.h"

/*
 * PICを初期化する
 */
void pic_init()
{
  // PICを初期化(ICW1)
  outb(PORT_MASTER_PIC_COMMAND, PIC_ICW1);
  outb(PORT_SLAVE_PIC_COMMAND , PIC_ICW1);
  // 割り込みベクタ情報通知(ICW2)
  outb(PORT_MASTER_PIC_DATA, PIC_MASTER_ICW2);
  outb(PORT_SLAVE_PIC_DATA , PIC_SLAVE_ICW2 );
  // IRライン情報通知(ICW3)
  outb(PORT_MASTER_PIC_DATA, PIC_MASTER_ICW3);
  outb(PORT_SLAVE_PIC_DATA , PIC_SLAVE_ICW3 );
  // 動作設定(ICW4)
  outb(PORT_MASTER_PIC_DATA, PIC_MASTER_ICW4);
  outb(PORT_SLAVE_PIC_DATA , PIC_SLAVE_ICW4 );
}

/*
 * I/Oポートでデータを送信する
 *
 * @param port 使用するポート
 * @param data 送信するバイト
 */
inline void outb(unsigned short port, unsigned char data)
{
  asm volatile("outb %0, %1" : : "a"(data), "dN"(port));
}

/*
 * I/Oポートでデータを受信する
 *
 * @param port 使用するポート
 */
u_char inb(u_short port)
{
  u_char data;
  asm volatile("inb %1, %0" : "=a"(data) : "dN"(port));
  return data;
}
