/*
 * irq.c - 割り込みハンドラの定義（例外を除く）
 */
#include "irq.h"
#include "hal.h"
#include "pit.h"
#include "../include/io.h"

/*
 * PITの割り込みハンドラ
 */
void __attribute__((__cdecl__)) irq_pit(void)
{
  enter_interrupt();

  pit_increment_tick();
  interrupt_done(0);

  fb_print("[DEBUG] IRQ_PIT\n");

  exit_interrupt();
}

/*
 * キーボード
 */
void __attribute__((__cdecl__)) irq_keyboard(void)
{
  __asm__ __volatile__("addl %esp, 12");
  __asm__ __volatile__("pusha");
  __asm__ __volatile__("cli");

  u_char code = 0;

  if (kb_ctrl_readstat() & KYBRD_CTRL_STATS_MASK_OUT_BUF) {
    code = kb_enc_readbuf();

    if (code == 0xE0 || code == 0xE1) {
    } else {
      kb_info.lastkey = code;
      fb_printx(kb_info.lastkey);
    }
  }

  interrupt_done(0);

  fb_print("[DEBUG] IRQ_Keyboard\n");

  __asm__ __volatile__("sti");
  __asm__ __volatile__("popa");
  __asm__ __volatile__("iret");
}
