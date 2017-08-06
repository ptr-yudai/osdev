/*
 * screen.c - 画面の管理
 */
#include "screen.h"
#include "../include/util.h"

/*
 * 画面初期化
 */
void screen_init(void)
{
  int i;
  scrmgr.scrsize = VGA_HEIGHT * VGA_WIDTH * 2;
  
  // 仮想画面のメモリ確保
  for(i = 0; i < SCR_VIRTUAL_MAX; i++) {
    scrmgr.vga[i].p_framebuffer = malloc(4);
    scrmgr.vga[i].fb_position = 0;
  }
  scrmgr.focus = 0;

  // 各画面を初期値で初期化
  for(i = 0; i < SCR_VIRTUAL_MAX; i++) {
    scr_switch(i);
    fb_setpos(0, 0);
    fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    fb_clrscr();
    scr_redraw();
  }
  scr_switch(0);
  
  // メニューを作成
  fb_move_cursor(1, 0);
}

/*
 * 画面を切り換える
 *
 * @param n 切り替え先番号
 */
void scr_switch(u_char n)
{
  VIRTUAL_VGA vvga_f;
  if (n >= SCR_VIRTUAL_MAX) return; // 範囲外
  if (n == scrmgr.focus) return;    // 切り替え済み
  // 初期化
  vvga_f = scrmgr.vga[n];
  // カーソルを入れ替え
  scrmgr.vga[scrmgr.focus].fb_position = fb_position;
  fb_position = vvga_f.fb_position;
  // 画面を入れ替え
  memcpy((void*)VGA_FRAMEBUFFER,
	 vvga_f.p_framebuffer,
	 scrmgr.scrsize);
  scrmgr.focus = n;
  // カーソルを再描画
  fb_redraw_cursor();
}

/*
 * メニューを描画
 */
void scr_draw_menu(void)
{
  u_int i;
  u_int bkup_fb_position = fb_position;
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_DARK_GREY, VGA_COLOR_BROWN);
  for(i = 0; i < VGA_WIDTH; i++) fb_putc(' ');
  fb_position = bkup_fb_position;
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
}

/*
 * 再描画
 */
void scr_redraw(void)
{
  u_char line[VGA_WIDTH * 2];
  VIRTUAL_VGA vvga;
  vvga = scrmgr.vga[scrmgr.focus];
  // スクロール処理
  if (fb_position > scrmgr.scrsize / 2) {
    // [TODO] ちゃんとスクロールすること
    memcpy((void*)line,
	   (void*)(scr_currentfb() + scrmgr.scrsize - VGA_WIDTH * 2),
	   VGA_WIDTH * 2);
    // (仮)画面を消すだけ
    fb_setpos(0, 0);
    fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    fb_clrscr();
    // 最後の一行は残す
    memcpy((void*)(scr_currentfb() + VGA_WIDTH * 2),
	   (void*)line,
	   VGA_WIDTH * 2);
    // メニューを表示
    scr_draw_menu();
    fb_move_cursor(2, 0);
  }
  // 画面を入れ替え
  memcpy((void*)VGA_FRAMEBUFFER,
	 vvga.p_framebuffer,
	 VGA_WIDTH * VGA_HEIGHT * 2);
  scr_switch(scrmgr.focus);
}

/*
 * 仮想フレームバッファへのポインタを取得する
 *
 * @return フォーカス中の仮想FBのアドレス
 */
void* scr_currentfb(void)
{
  return scrmgr.vga[scrmgr.focus].p_framebuffer;
}

/*
 * 現在フォーカス中の画面IDを取得する
 *
 * @return フォーカス中の仮想画面のID
 */
u_char scr_currentscr(void)
{
  return scrmgr.focus;
}
