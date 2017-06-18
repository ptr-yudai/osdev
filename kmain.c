#include "./include/io.h"
#include "./include/util.h"
#include "./hal/idt.h"
#include "./hal/hal.h"
#include "./hal/pic.h"
#include "./hal/pit.h"
#include "./hal/irq.h"

/*
 * カーネルメイン
 */
void kmain()
{
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();

  // 割り込みを設定
  idt_init();
  //pic_init();
  //pit_init();

  fb_print("[INFO] idtr.base = ");
  fb_printx((u_int)&idtr);
  fb_print("\n");

  enable_interrupt();

  for(;;) {
    fb_print(".");
    int i;
    for(i = 0; i < 10000; i++) {
      i = i;
    }
  }
  
  fb_print("\nCPU is going to halt. See you...\n");

  // ゼロ除算（例外割り込み）
  //int a = 1, b = 0;
  //a /= b;
}
