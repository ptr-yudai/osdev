#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./hal/hal.h"

/*
 * カーネルメイン
 */
void kmain()
{
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();

  // 割り込みを設定
  hal_init();

  char c[3] = "\x00\n\x00";
  int i;
  for(i = 0; i < 10; i++) {
    c[0] = kb_getc();
    fb_print(c);
  }
  
  fb_print("\nCPU is going to halt. See you...\n");
}
