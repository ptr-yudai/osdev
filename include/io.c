/*
 * io.c - 入出力関連
 */
#include "io.h"
#include "util.h"
#include "../hal/pic.h"
#include "../hal/key.h"

/*--------------------------------------------------*/
//
//  キー入力関連
//
/*--------------------------------------------------*/
/*
 * 1文字をキーボードから入力する
 *
 * @return 入力された文字コード
 */
u_char kb_getc(void)
{
  u_int key = KEY_UNKNOWN;
  // 入力待ち
  while(key == KEY_UNKNOWN || key == 0x0000) {
    key = kb_get_lastkey();
  }
  kb_info.lastkey = KEY_UNKNOWN;
  return key;
}


/*--------------------------------------------------*/
//
//  画面出力関連
//
/*--------------------------------------------------*/
/*
 * 1文字を特定のカーソルに書き込む
 * 
 * @param c  文字
 */
void fb_putc(u_char c)
{
  u_int frac;
  // 改行かどうか
  if (c == '\x0a') {
    // 改行する
    frac = fb_position % VGA_WIDTH;
    fb_position += VGA_WIDTH - frac;
  } else {
    // 構造体を構成する
    fb_vga.ascii = c;
    // メモリマップトI/Oで書き込み
    memcpy((void*)(0x000B8000 + fb_position * 2), &fb_vga, 2);
    // カーソルを進める
    fb_position++;
  }
}

/*
 * 文字列をカーソルに書き込む
 *
 * @param str 書き込む文字列
 */
void fb_print(const char* str)
{
  u_char* p_str;
  for(p_str = (u_char*)str; *p_str != 0; ++p_str) {
    fb_putc(*p_str);
  }
}

/*
 * 数字を16進数で表示する
 *
 * @param n 表示する数値
 */
void fb_printx(u_int n)
{
  u_short i;
  u_char c;
  u_int x;

  fb_putc('0'); fb_putc('x');
  for(x = n, i = 0; i < 8; x <<= 4, i++) {
    c = (u_char)((x & 0xF0000000) >> 28);
    // 0 - 9
    if (c < 10) c += 0x30;
    // a - f
    else if (c < 16) c += 0x37;
    // 到達不能
    else c = 0x3F;
    fb_putc(c);
  }
}

/*
 * 画面を現在の背景色で消去する
 */
void fb_clrscr(void)
{
  int i;
  // 80列, 25行を削除
  fb_move_cursor(0, 0);
  for(i = 0; i < 2000; i++) fb_putc(' ');
  fb_move_cursor(0, 0);
}

/*
 * 表示する色を設定する
 *
 * @param fg 文字色
 * @param bg 背景色
 */
void fb_setcolor(u_char fg, u_char bg)
{
  fb_vga.fg = fg & 0xF;
  fb_vga.bg = bg & 0xF;
}

/*
 * 表示位置を変更する（カーソル位置は変わらない）
 *
 * @param row    行番号
 * @param column 列番号
 */
u_int fb_setpos(u_int row, u_int column)
{
  fb_position = VGA_WIDTH * row + column;
  return fb_position;
}

/*
 * カーソルを移動する（表示位置も変更）
 *
 * @param row    行番号
 * @param column 列番号
 */
void fb_move_cursor(u_int row, u_int column)
{
  u_int pos = fb_setpos(row, column);
  // 文字は1行で80バイトまで
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT   , (pos >> 8) & 0x00FF );
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND );
  outb(FB_DATA_PORT   , pos & 0x00FF);
}
