#ifndef SYS_SCREEN_H
#define SYS_SCREEN_H

#include "../include/types.h"
#include "../include/io.h"
#include "../include/util.h"
#include "../hal/mem.h"

/*----- 定数定義 -----*/
// 最大画面数
#define SCR_VIRTUAL_MAX 4
// 
#define SCR_ALLOCATED_SCR  4
#define SCR_ALLOCATED_LINE ((SCR_ALLOCATED_SCR * MEMORY_BLOCK_SIZE) / (80 * 2))
// 画面の役割
#define SCR_SYSTEM_MSG  0  // OSレベルのデバッグメッセージ
#define SCR_USER_ERROR  1  // ユーザーエラー
#define SCR_PROGRAM     2  // プログラムの出力
#define SCR_TEMPORARY   3  // 一時的な使用

/*----- 構造体定義 ------*/
typedef struct {
  u_int fb_position;
  void* p_framebuffer; // 仮想画面へのポインタ
  void* p_current_fb;  // 現在見える仮想画面へのポインタ
  u_short current_line;  // 現在の行
  u_short start_line;    // 開始行（それ以上遡れない）
  u_short end_line;      // 終了行（それ以上下れない）
} VIRTUAL_VGA;
typedef struct /*__attribute__((packed))*/ {
  VIRTUAL_VGA vga[SCR_VIRTUAL_MAX]; // 仮想画面へのポインタ
  u_char focus;                     // 現在表示中の画面番号
  u_int scrsize;                    // フレームバッファのバイト数
} SCR_VIRTUAL;

/*----- 変数定義 ------*/
SCR_VIRTUAL scrmgr;

/*----- 関数定義 -----*/
void screen_init(void);
void scr_switch(u_char n);
void scr_draw_menu(void);
void scr_redraw(void);
void scr_move_cline(short line);
void* scr_currentfb(void);
u_char scr_currentscr(void);

#endif
