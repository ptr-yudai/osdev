/*
 * screen.c - 画面の管理
 */
#include "screen.h"

/*
 * 画面初期化
 */
void screen_init(void)
{
  //int i;
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();

  // 仮想画面のメモリ確保
  /*
  for(i = 0; i < SCR_VIRTUAL_MAX; i++) {
    scrmgr.vga[i].p_framebuffer = mem_alloc_blocks(1);
    scrmgr.vga[i].fb_position = 0;
  }
  scrmgr.focus = 0;

  // 各画面を初期値で初期化
  for(i = 0; i < SCR_VIRTUAL_MAX; i++) {
    fb_setpos(0, 0);
    fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    fb_clrscr();
    scr_switch(i);
  }
  scr_switch(0);
  
  // メニューを作成
  scr_draw_menu();
  fb_move_cursor(1, 0);
  */
}

/*
 * 画面を切り換える
 *
 * @param n 切り替え先番号
 */
void scr_switch(u_char n)
{
  VIRTUAL_VGA vvga_b, vvga_f;
  if (n >= SCR_VIRTUAL_MAX) return; // 範囲外
  if (n == scrmgr.focus) return;    // 切り替え済み
  // 初期化
  vvga_b = scrmgr.vga[scrmgr.focus];
  vvga_f = scrmgr.vga[n];
  // カーソルを入れ替え
  vvga_b.fb_position = fb_position;
  fb_position = vvga_f.fb_position;
  // 画面を入れ替え
  memcpy(vvga_b.p_framebuffer,
	 (void*)VGA_FRAMEBUFFER,
	 VGA_WIDTH * VGA_HEIGHT * 2);
  memcpy((void*)VGA_FRAMEBUFFER,
	 vvga_f.p_framebuffer,
	 VGA_WIDTH * VGA_HEIGHT * 2);
  scrmgr.focus = n;
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