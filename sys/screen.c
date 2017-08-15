/*
 * screen.c - 画面の管理
 */
#include "screen.h"
#include "../include/util.h"
#include "../include/time.h"

/*
 * 画面初期化
 */
void screen_init(void)
{
  int i;
  scrmgr.scrsize = VGA_HEIGHT * VGA_WIDTH * 2;
  
  // 仮想画面のメモリ確保
  for(i = 0; i < SCR_VIRTUAL_MAX; i++) {
    scrmgr.vga[i].p_framebuffer = malloc(SCR_ALLOCATED_SCR);
    scrmgr.vga[i].fb_position = 0;
    scrmgr.vga[i].p_current_fb = scrmgr.vga[i].p_framebuffer;
    scrmgr.vga[i].current_line = 0;
    scrmgr.vga[i].start_line = 0;
    scrmgr.vga[i].end_line = 0;
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
  fb_setcolor(VGA_COLOR_RED, VGA_COLOR_BROWN);
  for(i = 0; i < VGA_WIDTH; i++) fb_putc(' ');
  fb_setpos(0, 0);
  fb_print("F1:DEBUG  F2:SHELL  F3:PROGRAM  F4:TEMP");
  fb_position = bkup_fb_position;
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
}

/*
 * 再描画
 */
void scr_redraw(void)
{
  VIRTUAL_VGA *vvga;
  vvga = &scrmgr.vga[scrmgr.focus];

  // 画面を入れ替え
  memcpy((void*)VGA_FRAMEBUFFER,
	 vvga->p_current_fb,
	 VGA_WIDTH * VGA_HEIGHT * 2);
  scr_switch(scrmgr.focus);
  fb_redraw_cursor();
}

/*
 * 現在表示する行を移動する
 *
 * @param line 移動する行（相対値）
 */
void scr_move_cline(short line)
{
  VIRTUAL_VGA *vvga;
  vvga = &scrmgr.vga[scrmgr.focus];

  // 履歴範囲外かをチェック
  if (vvga->current_line + line > vvga->start_line) return;
  if (vvga->current_line + line < vvga->end_line) return;
  
  // 移動
  vvga->p_current_fb = (void*)((char*)vvga->p_current_fb
			       + line * VGA_WIDTH * 2);
  vvga->current_line += line;
  
  vvga->fb_position -= line * VGA_WIDTH;
  fb_position = vvga->fb_position;
  
  //scr_draw_menu();
  fb_redraw_cursor();
}

/*
 * 仮想フレームバッファへのポインタを取得する
 *
 * @return フォーカス中の仮想FBのアドレス
 */
void* scr_currentfb(void)
{
  return scrmgr.vga[scrmgr.focus].p_current_fb;
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
