#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- 定数 -----*/
/* VGA設定 */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
enum VGA_COLOR {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};
/* I/Oポート */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT    0x3D5
/* I/Oポートのコマンド */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15
/* キーボード */
enum KYBRD_ENCODER_IO {
  KYBRD_ENC_INPUT_BUF = 0x60,
  KYBRD_ENC_CMD_REG   = 0x60
};
enum KYBRD_CTRL_IO {
  KYBRD_CTRL_STATS_REG = 0x64,
  KYBRD_CTRL_CMD_REG   = 0x64
};
// 入力を待つための状態レジスタマスク
enum KYBRD_CTRL_STATS_MASK {
  KYBRD_CTRL_STATS_MASK_OUT_BUF  = 1,
  KYBRD_CTRL_STATS_MASK_IN_BUF   = 2,
  KYBRD_CTRL_STATS_MASK_SYSTEM   = 4,
  KYBRD_CTRL_STATS_MASK_CMD_DATA = 8,
  KYBRD_CTRL_STATS_MASK_LOCKED   = 0x10,
  KYBRD_CTRL_STATS_MASK_AUX_BUF  = 0x20,
  KYBRD_CTRL_STATS_MASK_TIMEOUT  = 0x40,
  KYBRD_CTRL_STATS_MASK_PARITY   = 0x80
};
// キーコード
enum KEYCODE {
  KEY_SPACE = ' ',
  KEY_UNKNOWN,
  KEY_UNSET
}

/*----- 構造体 -----*/
// VGA(フレームバッファ)
typedef struct __attribute__((packed)) {
  u_char ascii;
  u_char fg: 4;
  u_char bg: 4;
} VGA_CHAR;
// キーボード入力情報
typedef struct {
  u_char lastkey;  // 最後にスキャンされた文字コード
} KEYBOARD_INFO;

/*----- 関数定義 -----*/
// キー入力
u_char kb_getc(void);
u_char kb_ctrl_readstat(void);
u_char kb_end_readbuf(void);
void kb_ctrl_sendcmd(u_char cmd);
void kb_enc_sendcmd(u_char cmd);
u_char kb_get_lastkey(void);
u_char kb_lookup_keycode(u_char keycode);
// 画面出力
void fb_putc(u_char c);
void fb_print(const char* str);
void fb_printx(u_int n);
void fb_clrscr(void);
void fb_setcolor(u_char fg, u_char bg);
u_int fb_setpos(u_int row, u_int column);
void fb_move_cursor(u_int row, u_int column);

/*----- グローバル変数 -----*/
VGA_CHAR fb_vga;
KEYBOARD_INFO kb_info;
u_int fb_position;

#endif
