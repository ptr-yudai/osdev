/*
 * irq.c - 割り込みハンドラの定義（例外を除く）
 */
#include "irq.h"
#include "../include/io.h"

/*
 * キーボード
 */
void _cdecl irq_keyboard(void)
{
  __asm__ __volatile__("add esp, 12");
  __asm__ __volatile__("pushad");
  __asm__ __volatile__("cli");

  u_char code = 0;

  if (kb_ctrl_readstat() & KYBRD_CTRL_STATS_MASK_OUT_BUF) {
    code = kb_enc_readbuf();

    if (code == 0xE0 || code == 0xE1) {
    } else {
      
    }
  }

  __asm__ __volatile__("sti");
  __asm__ __volatile__("popad");
  __asm__ __volatile__("iretd");
}
