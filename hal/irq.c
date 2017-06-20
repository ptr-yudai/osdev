/*
 * irq.c - 割り込みハンドラの定義（例外を除く）
 */
#include "irq.h"
#include "hal.h"
#include "pit.h"
#include "key.h"
#include "../include/io.h"

/*
 * PITの割り込みハンドラ
 */
void irq_pit(void)
{
  pit_increment_tick();
  interrupt_done(0);
  //fb_print("[DEBUG] IRQ_PIT\n");
}

/*
 * キーボード
 */
void irq_keyboard(void)
{
  u_char code = 0;

  if (kb_ctrl_readstat() & KYBRD_CTRL_STATS_MASK_OUT_BUF) {
    // キーコードを読み込む
    code = kb_enc_readbuf();

    if (code == 0xE0 || code == 0xE1) {
    } else {
      kb_info.lastkey = code;
      fb_printx(kb_lookup_keycode(kb_info.lastkey));
      fb_print("\n");
    }
  }

  interrupt_done(0);

  //fb_print("[DEBUG] IRQ_Keyboard\n");
}
