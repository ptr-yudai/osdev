#include "./include/io.h"
#include "./include/util.h"
#include "./hal/idt.h"
#include "./hal/pic.h"

void mode_protect(void);

/*
 * カーネルメイン
 */
void kmain()
{
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();

  fb_print("It's running on protected mode!\n");

  // 割り込みを設定
  idt_init();
  pic_init();
  
  fb_print("\nCPU is going to halt. See you...\n");

  // ゼロ除算（例外割り込み）
  //int a = 1, b = 0;
  //a /= b;
}
