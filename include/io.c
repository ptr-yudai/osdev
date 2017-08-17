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
/**
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
    __asm__ __volatile__("hlt");
  }
  kb_info.lastkey = KEY_UNKNOWN;
  return key;
}

/**
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
      *p_result = 0;
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
  fb_chkscroll();
  fb_redraw_cursor();
}

/**
 * 数字を入力する
 *
 * @param num 数字を格納するポインタ
 * @return 正しい入力なら1
 */
u_char kb_getnumber(int *num)
{
  u_char sign = 1;
  int i;
  char str[12]; // intは10行程度
  // [TODO] オーバーフローを直す
  kb_getline(str);
  // 数値に変換
  *num = 0;
  for(i = 0; i < 12; i++) {
    if (isdigit(str[i])) {
      // 数字を変換
      *num *= 10;
      *num += (int)(str[i] - 0x30);
    } else {
      // NULLなら終了
      if (str[i] == 0) break;
      // 符号は許す
      if (i == 0 && str[i] == '-') {
	// 符号だけは許さない
	if (isdigit(str[1]) == 0) return 0;
	sign = -1;
      } else {
	return 0;
      }
    }
    if (i == 11) return 0;
  }
  *num *= sign;
  
  return 1;
}
  
/*--------------------------------------------------*/
//
//  画面出力関連
//
/*--------------------------------------------------*/
/**
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

  fb_chkscroll();
  fb_redraw_cursor();
}

/**
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

/**
 * エラーをカーソルに書き込む
 *
 * @param str 書き込むエラーメッセージ
 * @param lev エラーレベル
 */
void fb_debug(const char* str, char lev)
{
  u_char scr = scr_currentscr();
  scr_switch(SCR_USER_ERROR);
  switch(lev) {
  case ER_INFO:
    fb_print("[MESSAGE] ");
    break;
  case ER_DEBUG:
    fb_print("[ DEBUG ] ");
    break;
  case ER_CATION:
    fb_print("[ ERROR ] ");
    break;
  case ER_WARNING:
    fb_print("[WARNING] ");
    break;
  case ER_FATAL:
    fb_print("[ FATAL ] ");
    break;
  default:
    fb_print("[NOTHING] ");
    break;
  }
  fb_print(str);
  scr_switch(scr);
}

/**
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

/**
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

/**
 * バイナリデータをダンプする
 *
 * @param bin  バイナリデータ
 * @param size 表示サイズ
 */
void fb_hexdump(char *bin, u_int size)
{
  u_int i;
  char digit[2];
  fb_print("+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F\n");
  for(i = 0; i < size; ++i) {
    itoa(bin[i] >> 4  , digit, 16);
    fb_putc(digit[0]);
    itoa(bin[i] & 0x0F, digit, 16);
    fb_putc(digit[0]);
    fb_putc(' ');
    if ((i + 1) % 16 == 0) fb_print("\n");
  }
}

/**
 * フォーマット文字列を表示する
 */
void fb_printf(char *format, ...)
{
  va_list argptr;
  char s[64], c, *str;
  int num;
  
  va_start(argptr, format);
  
  while(*format != '\0') {
    if (*format != '%') {
      fb_putc(*format);
    } else {
      format++;
      switch(*format) {
      case '%':
	fb_putc('%');
	break;
      case 'b':
	num = va_arg(argptr, int);
	itoa(num, s, 2);
	fb_print(s);
	break;
      case 'c':
	c = va_arg(argptr, char);
	fb_putc(c);
	break;
      case 's':
	str = va_arg(argptr, char *);
	fb_print(str);
	break;
      case 'x':
	num = va_arg(argptr, int);
	itoa(num, s, 16);
	fb_print(s);
	break;
      case 'd':
	num = va_arg(argptr, int);
	itoa(num, s, 10);
	fb_print(s);
	break;
      default:
	fb_putc(*(--format));
	fb_putc(*(++format));
	break;
      }
    }
    format++;
  }
  va_end(argptr);
}

/**
 * 画面を現在の背景色で消去する
 */
void fb_clrscr(void)
{
  int i;
  // 80列, 25行を削除
  fb_move_cursor(0, 0);
  for(i = 0; i < VGA_WIDTH * VGA_HEIGHT * SCR_ALLOCATED_SCR; i++) {
    fb_putc(' ');
  }
  // メニューを描画
  scr_draw_menu();
  fb_move_cursor(1, 0);
}

/**
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

/**
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

/**
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

/**
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

/**
 * スクロールをチェックする
 */
void fb_chkscroll()
{
  // カーソルが画面外ならスクロール
  VIRTUAL_VGA *vvga;
  vvga = &scrmgr.vga[scrmgr.focus];
  if (fb_position > scrmgr.scrsize / 2) {
    vvga->start_line++;
  }
}
