/*
 * io.c - 入出力関連
 */
#include "io.h"
#include "util.h"
#include "../hal/pic.h"
#include "../hal/key.h"
#include "../sys/screen.h"

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
  while(key == KEY_UNKNOWN || key == 0x00) {
    key = kb_get_lastkey();
    key = kb_key2ascii_jp(key);
  }
  kb_info.lastkey = KEY_UNKNOWN;
  return key;
}

/*
 * 改行までをキーボードから入力する
 *
 * @param *str 入力を格納するポインタ
 */
void kb_getline(char* str)
{
  char buf[2] = "\x00\x00";
  char* p_result = str;
  char* p_tmp;
  char is_end = 1;
  while(is_end) {
    // 入力して表示
    buf[0] = kb_getc();
    switch(buf[0]) {
    case KEY_RETURN:
      // ENTERなら終了
      fb_print("\n");
      is_end = 0;
      break;
    case KEY_BACKSPACE:
      // 1文字目は無視
      if (p_result <= str) break;
      // BACKSPACEなら削除して詰める
      for(p_tmp = p_result--; *p_tmp; p_tmp++) {
	*p_tmp = *(p_tmp + 1);
      }
      *p_result = 0;
      // 再表示
      fb_position--; fb_redraw_cursor();
      fb_putc(' ');
      fb_position--; fb_redraw_cursor();
      break;
    default:
      // 普通の文字
      fb_print(buf);
      *p_result = buf[0];
      p_result++;
    }
  }
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
    memcpy((void*)(scr_currentfb() + fb_position * 2), &fb_vga, 2);
    // カーソルを進める
    fb_position++;
  }
  fb_redraw_cursor();
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
 * バイナリをカーソルに書き込む
 *
 * @param bin  書き込むバイナリ
 * @param size 書き込みバイト数
 */
void fb_printb(char* bin, u_int size)
{
  u_int i;
  u_char* p_bin = (u_char*)bin;
  u_char c;
  for(i = 0; i < size; i++, p_bin++) {
    fb_putc('\\'); fb_putc('x');
    // 上位4ビット
    c = (*p_bin & 0xF0) >> 4;
    if (c < 10) c += 0x30;
    else if (c < 16) c += 0x37;
    else c = 0x3F;
    fb_putc(c);
    // 下位4ビット
    c = (*p_bin & 0x0F);
    if (c < 10) c += 0x30;
    else if (c < 16) c += 0x37;
    else c = 0x3F;
    fb_putc(c);
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
  fb_setpos(row, column);
  fb_redraw_cursor();
}

/*
 * カーソルを更新する
 */
void fb_redraw_cursor(void)
{
  // 文字は1行で80バイトまで
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT   , (fb_position >> 8) & 0x00FF );
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND );
  outb(FB_DATA_PORT   , fb_position & 0x00FF);
}
