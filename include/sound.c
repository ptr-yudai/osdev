#include "sound.h"

/*
 * ビープ音を鳴らす
 *
 * @param freq 周波数
 * @param t    時間
 */
void play_beep(u_int freq, u_int t)
{
  u_int div;
  u_char tmp;
  // セットアップ
  div = 1193180 / freq;
  outb(0x43, 0xB6);
  outb(0x42, (u_char)(div & 0xFF));
  outb(0x42, (u_char)(div >> 8));
  // 鳴らす
  tmp = inb(0x61);
  if (tmp != (tmp | 3)) {
    outb(0x61, tmp | 3);
  }
  // 待つ
  sleep(t);
  // 止める
  tmp = inb(0x61) & 0xFC;
  outb(0x61, tmp);
}

