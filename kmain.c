#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./hal/hal.h"

/*
 * カーネルメイン
 */
void kmain()
{
  char c[64];

  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();

  // 割り込みを設定
  hal_init();

  kb_getline(c);

  fb_print(c);
  
  fb_print("\nCPU is going to halt. See you...\n");
}
