#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./hal/hal.h"

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
  hal_init();

  fb_print("Magic Number: ");
  fb_printx(magic);
  fb_print("\nmbd->mem_lower  = ");
  fb_printx(mbd->mem_lower);
  fb_print("\nmbd->mem_upper  = ");
  fb_printx(mbd->mem_upper);
  fb_print("\nmbd->mmap_length = ");
  fb_printx(mbd->mmap_length);
  fb_print("\nmbd->mmap_addr  = ");
  fb_printx(mbd->mmap_addr);
  fb_print("\n");
  
  fb_print("\nCPU is going to halt. See you...\n");
}
