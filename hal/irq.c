/*
 * irq.c - 割り込みハンドラの定義（例外を除く）
 */
#include "irq.h"
#include "hal.h"
#include "pit.h"
#include "key.h"
#include "../sys/screen.h"
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
      if (code & 0x80) {
	// キー離す
	code -= 0x80;
	switch(kb_lookup_keycode(code)) {
	case KEY_LCTRL: // CTRLキー離す
	case KEY_RCTRL:
	  kb_info.flg_ctrl = 0;
	  break;
	case KEY_LSHIFT: // SHIFTキー離す
	case KEY_RSHIFT:
	  kb_info.flg_shift = 0;
	  break;
	case KEY_LALT: // ALTキー離す
	case KEY_RALT:
	  kb_info.flg_alt = 0;
	  break;
	}
      } else {
	// キー押す
	kb_info.lastkey = code;

	//fb_printx(code);

	switch(kb_lookup_keycode(code)) {
	case KEY_LCTRL:      // CTRLキー押す
	case KEY_RCTRL:
	  kb_info.flg_ctrl = 1;
	  break;
	case KEY_LSHIFT:     // SHIFTキー押す
	case KEY_RSHIFT:
	  kb_info.flg_shift = 1;
	  break;
	case KEY_LALT:       // ALTキー押す
	case KEY_RALT:
	  kb_info.flg_alt = 1;
	  break;
	case KEY_CAPSLOCK:   // CAPSキー押す
	  kb_info.flg_caps = ~kb_info.flg_caps;
	  break;
	case KEY_SCROLLLOCK: // SCROLLキー押す
	  kb_info.flg_scr = ~kb_info.flg_scr;
	  break;
	  
	case KEY_F1: // 仮想画面1
	  scr_switch(0);
	  break;
	case KEY_F2: // 仮想画面2
	  scr_switch(1);
	  break;
	case KEY_F3: // 仮想画面3
	  scr_switch(2);
	  break;
	case KEY_F4: // 仮想画面4
	  scr_switch(3);
	  break;
	}
      }
    }
  }

  //fb_print("[DEBUG] IRQ_Keyboard\n");

  interrupt_done(0);
}
