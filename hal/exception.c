#include "exception.h"
#include "../include/io.h"

/*
 * カーネルパニック画面への移行
 * （この関数が呼び出されるとCPUは完全に停止します）
 */
void kernel_panic(const char* msg)
{
  fb_move_cursor(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_RED);
  fb_clrscr();
  fb_move_cursor(0, 33);
  fb_print("[KERNEL PANIC]");
  fb_move_cursor(1, 0);
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

/*
 * 例外ハンドラ：
 */
void handler_divided_by_zero()
{
  kernel_panic("Divided By Zero");
}
