#include "./include/io.h"
#include "./include/util.h"

void mode_protect(void);

/*
 * カーネルメイン
 */
void kmain()
{
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
  fb_print("It's running on protected mode!\n");
  
  

  fb_print("\nCPU is going to halt. See you...\n");
}
