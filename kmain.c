#include "./include/io.h"
#include "./include/util.h"
#include "./hal/idt.h"

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

  idt_init();
  
  fb_print("\nCPU is going to halt. See you...\n");

  //idt_genint(0);
}
