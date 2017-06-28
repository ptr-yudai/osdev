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
  fb_printx(magic); fb_print("\n");

  fb_print("Bitmap table is located at ");
  fb_printx((u_int)pm_info.mem_map); fb_print("\n");

  // メモリ割り当てテスト
  int i;
  void *p[10];
  for(i = 0; i < 5; i++) {
    p[i] = mem_alloc_block();
    fb_print("alloc --> "); fb_printx((u_int)p[i]); fb_print("\n");
  }
  for(i = 2; i < 5; i++) {
    mem_free_block(p[i]);
    fb_print("free --> "); fb_printx((u_int)p[i]); fb_print("\n");
  }
  for(i = 5; i < 10; i++) {
    p[i] = mem_alloc_block();
    fb_print("alloc --> "); fb_printx((u_int)p[i]); fb_print("\n");
  }
  
  
  fb_print("\nCPU is going to halt. See you...\n");
}
