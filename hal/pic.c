/*
 * pic.c - PICの操作関数
 */
#include "pic.h"
#include "hal.h"
#include "../include/io.h"

/*
 * PICを初期化する
 */
void pic_init()
{
  u_char irq_mask;
  disable_interrupt();

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
  // IRQ0(PIT)とIRQ1(Key)だけを有効化
  irq_mask = PIC_IMR_MASK_IRQ_ALL;
  irq_mask &= ~PIC_IMR_MASK_IRQ0;
  irq_mask &= ~PIC_IMR_MASK_IRQ1;
  outb(PORT_MASTER_PIC_IMR, irq_mask);
  outb(PORT_SLAVE_PIC_IMR , PIC_IMR_MASK_IRQ_ALL);

  fb_print("[DEBUG] PIC init\n");
}

/*
 * PICにコマンドを送る
 *
 * @param cmd  コマンド
 * @param picn PICの番号
 */
void pic_sendcmd(u_char cmd, u_char picn)
{
  if (picn > 1) return;
  u_char reg;
  if (picn == 1) {
    reg = PIC2_REG_COMMAND;
  } else {
    reg = PIC1_REG_COMMAND;
  }
  outb(reg, cmd);
}

/*
 * PICにデータを送る
 *
 * @param data データ
 * @param picn PICの番号
 */
void pic_senddata(u_char data, u_char picn)
{
  if (picn > 1) return;
  u_char reg;
  if (picn == 1) {
    reg = PIC2_REG_DATA;
  } else {
    reg = PIC1_REG_DATA;
  }
  outb(reg, data);
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
