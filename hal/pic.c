#include "pic.h"



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
