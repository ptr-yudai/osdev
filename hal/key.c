/*
 * key.c - キーボード入力
 */
#include "key.h"
#include "pic.h"
#include "idt.h"
#include "irq.h"
#include "../include/util.h"

// キーボードとキーコードの対応表
/*
static u_int kb_scancode_us[] = {
  KEY_UNKNOWN,
  KEY_ESCAPE,
  KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
  KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE, KEY_TAB,
  KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
  KEY_LEFTBRACKET, KEY_RIGHTBRACKET, KEY_RETURN, KEY_LCTRL,
  KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,
  KEY_SEMICOLON, KEY_QUOTE, KEY_GRAVE, KEY_LSHIFT, KEY_BACKSLASH,
  KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
  KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_RSHIFT, KEY_KP_ASTERISK,
  KEY_RALT, KEY_SPACE, KEY_CAPSLOCK,
  KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
  KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
  KEY_KP_NUMLOCK, KEY_SCROLLLOCK, KEY_HOME, KEY_KP_8,
  KEY_PAGEUP, KEY_KP_2, KEY_KP_3, KEY_KP_0, KEY_KP_DECIMAL,
  KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
  KEY_F11, KEY_F12
};
*/
static u_int kb_scancode_jp[] = {
  // 0x00 - 0x10
  KEY_UNKNOWN,
  KEY_ESCAPE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7,
  KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_CARRET, KEY_BACKSPACE, KEY_TAB, KEY_Q,
  // 0x11 - 0x20
  KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O,
  KEY_P, KEY_AT, KEY_LEFTBRACKET, KEY_RETURN, KEY_LCTRL, KEY_A, KEY_S, KEY_D,
  // 0x21 - 0x30
  KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_COLON,
  KEY_BACKSLASH, KEY_LSHIFT, KEY_RIGHTBRACKET, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B,
  // 0x31 - 0x40
  KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_BACKSLASH, KEY_KP_ASTERISK, KEY_RALT,
  KEY_SPACE, KEY_CAPSLOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
  // 0x41 - 0x50
  KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_KP_NUMLOCK, KEY_SCROLLLOCK, KEY_HOME, KEY_KP_8,
  KEY_PAGEUP, KEY_KP_2, KEY_KP_3, KEY_KP_0, KEY_KP_DECIMAL, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
  // 0x51 - 0x52
  KEY_F11, KEY_F12
};

/*
 * キーボードを初期化する
 */
void kb_init(void)
{
  // キー情報を初期化
  memset(&kb_info, 0, sizeof(kb_info));
  kb_info.lastkey = KEY_UNKNOWN;
  // IRQを設定
  idt_setup_ir(33, irq_keyboard_bridge);
}

/*
 * キーボードコントローラに1バイト送信する
 *
 * @param cmd 送信するコマンド
 */
void kb_ctrl_sendcmd(u_char cmd)
{
  // バッファが空になるのを待つ
  while(!(kb_ctrl_readstat() & KYBRD_CTRL_STATS_MASK_IN_BUF));
  outb(KYBRD_CTRL_CMD_REG, cmd);
}

/*
 * キーボードエンコーダに1バイト送信する
 *
 * @param cmd 送信するコマンド
 */
void kb_enc_sendcmd(u_char cmd)
{
  // バッファが空になるのを待つ
  while(!(kb_ctrl_readstat() & KYBRD_CTRL_STATS_MASK_IN_BUF));
  outb(KYBRD_ENC_CMD_REG, cmd);
}

/*
 * キーボードコントローラの状態を取得する
 *
 * @return in命令から取得した状態
 */
u_char kb_ctrl_readstat(void)
{
  return inb(KYBRD_CTRL_STATS_REG);
}

/*
 * キーボードエンコーダのバッファから取り出す
 *
 * @return in命令から取得したバイト
 */
u_char kb_enc_readbuf(void)
{
  return inb(KYBRD_ENC_INPUT_BUF);
}

/*
 * 最後の入力文字を取得
 *
 * @return 最後に入力された文字コード
 */
u_short kb_get_lastkey(void)
{
  u_int key = KEY_UNKNOWN;
  if (kb_info.lastkey != KEY_UNKNOWN) { 
    key = kb_lookup_keycode(kb_info.lastkey);
  }
  return key;
}

/*
 * キーコードを文字コードに変換する(jp)
 *
 * @param keycode キーコード
 * @return 文字コード
 */
u_int kb_lookup_keycode(u_char keycode)
{
  return kb_scancode_jp[keycode];
}

/*
 * 文字コードを大文字等を考慮したアスキーコードに変換する(jp)
 *
 * @param code 文字コード
 * @return アスキーコード
 */
u_char kb_key2ascii_jp(u_int code)
{
  u_char key = 0x00;
  if (isascii(code)) {
    key = code;
    // アルファベットを大文字にする
    if (kb_info.flg_shift || kb_info.flg_caps) {
      if (key >= 'a' && key <= 'z') key -= 0x20;
    }
    // シフトで記号にする
    if (kb_info.flg_shift) {
      switch(key) {
      case '1': key = KEY_EXCLAMATION; break;
      case '2': key = KEY_QUOTEDOUBLE; break;
      case '3': key = KEY_HASH; break;
      case '4': key = KEY_DOLLAR; break;
      case '5': key = KEY_PERCENT; break;
      case '6': key = KEY_AMPERSAND; break;
      case '7': key = KEY_QUOTE; break;
      case '8': key = KEY_LEFTPARENTHESIS; break;
      case '9': key = KEY_RIGHTPARENTHESIS; break;
      case KEY_MINUS : key = KEY_EQUAL; break;
      case KEY_CARRET: key = KEY_TILDE; break;
      case KEY_SLASH : key = KEY_QUESTION; break;
      case KEY_AT    : key = KEY_GRAVE; break;
      case KEY_LEFTBRACKET : key = KEY_LEFTCURL; break;
      case KEY_SEMICOLON: key = KEY_PLUS; break;
      case KEY_COLON : key = KEY_ASTERISK; break;
      case KEY_RIGHTBRACKET: key = KEY_RIGHTCURL; break;
      case KEY_COMMA : key = KEY_LESS; break;
      case KEY_DOT : key = KEY_GREATER; break;
      }
    }
  }
  return key;
}
