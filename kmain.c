#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/sound.h"
#include "./include/util.h"
#include "./sys/screen.h"
#include "./sys/shell.h"
#include "./hal/hal.h"

/*
 * カーネルメイン
 */
void kmain(multiboot_info_t* mbd, u_int magic)
{
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();
  // 割り込みを設定
  hal_init(mbd);
  // 画面管理を初期化
  screen_init();

  // 各種情報を表示
  fb_printf("[INFO] Magic Number: 0x%x\n", magic);
  fb_printf("[INFO] Memory Size: %d MB\n", mbd->mem_upper / 1024);

  // コマンド入力
  k_shell();

  // 終わり
  scr_switch(0);
  fb_debug("CPU is going to halt. See you...\n", ER_DEBUG);
}
