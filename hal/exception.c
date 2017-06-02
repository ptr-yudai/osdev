#include "exception.h"
#include "../include/io.h"

void kernel_panic(const char* msg)
{
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_RED);
  fb_clrscr();
  fb_setpos(0, 33);
  fb_print("[KERNEL PANIC]");
  fb_setpos(1, 0);
  fb_print("A fatal problem has been detected! \\(^o^)/\nERROR: ");
  fb_print(msg);
  
  asm volatile
    (
     ".loop:\n"
     "cli\n"
     "hlt\n"
     "jmp .loop\n"
     );
}
