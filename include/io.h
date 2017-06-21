#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include "types.h"

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

/*----- 構造体 -----*/
// VGA(フレームバッファ)
typedef struct __attribute__((packed)) {
  u_char ascii;
  u_char fg: 4;
  u_char bg: 4;
} VGA_CHAR;

/*----- 関数定義 -----*/
// 文字入力
u_char kb_getc(void);
void kb_getline(char* str);
// 画面出力
void fb_putc(u_char c);
void fb_print(const char* str);
void fb_printx(u_int n);
void fb_clrscr(void);
void fb_setcolor(u_char fg, u_char bg);
u_int fb_setpos(u_int row, u_int column);
void fb_move_cursor(u_int row, u_int column);
// サウンド
//void snd_playfreq(u_int freq);

/*----- グローバル変数 -----*/
VGA_CHAR fb_vga;
u_int fb_position;

#endif
