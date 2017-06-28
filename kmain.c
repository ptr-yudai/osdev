#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./include/linker.h"
#include "./hal/hal.h"
#include "./hal/mem.h"

/*
 * カーネルメイン
 */
void kmain(multiboot_info_t* mbd, u_int magic)
{
  // 画面初期化
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();

  // 割り込みを設定
  hal_init(mbd);

  fb_print("Magic Number: ");
  fb_printx(magic);
  fb_print("\n");
  fb_printx((u_int)(__KERNEL_TOP + sizeof_kernel()));
  fb_print("\n");
  
  fb_print("\nCPU is going to halt. See you...\n");
}
