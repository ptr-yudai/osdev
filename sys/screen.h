#ifndef SYS_SCREEN_H
#define SYS_SCREEN_H

#include "../include/types.h"
#include "../include/io.h"
#include "../include/util.h"
#include "../hal/mem.h"

/*----- 定数定義 -----*/
#define SCR_VIRTUAL_MAX 4

/*----- 構造体定義 ------*/
typedef struct {
  u_int fb_position;
  void* p_framebuffer; // 仮想画面へのポインタ
} VIRTUAL_VGA;
typedef struct /*__attribute__((packed))*/ {
  VIRTUAL_VGA vga[SCR_VIRTUAL_MAX]; // 仮想画面へのポインタ
  u_char focus;                     // 現在表示中の画面番号
} SCR_VIRTUAL;

/*----- 変数定義 ------*/
SCR_VIRTUAL scrmgr;

/*----- 関数定義 -----*/
void screen_init(void);
void scr_switch(u_char n);
void scr_draw_menu(void);
void scr_redraw(void);
void* scr_currentfb(void);

#endif
