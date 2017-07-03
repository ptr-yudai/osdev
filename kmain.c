#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./sys/screen.h"
#include "./hal/hal.h"
#include "./hal/vmem.h"

/*
 * カーネルメイン
 */
void kmain(multiboot_info_t* mbd, u_int magic)
{
  // 画面を初期化
  screen_init();

  // 割り込みを設定
  hal_init(mbd);

  // 各種情報を表示
  ///*
  fb_print("Magic Number: ");
  fb_printx(magic); fb_print("\n");
  fb_print("Memory Size: ");
  fb_printx(mbd->mem_upper / 1024); fb_print(" MB\n");
  //*/

  PT_ENTRY pte1;
  vmem_alloc_page(&pte1); fb_print("\n");
  fb_printx((u_int)pte1);
  PT_ENTRY pte2;
  vmem_alloc_page(&pte2); fb_print("\n");
  fb_printx((u_int)pte2);
  vmem_free_page(&pte2);
  PT_ENTRY pte3;
  vmem_alloc_page(&pte3); fb_print("\n");
  fb_printx((u_int)pte3);
  PT_ENTRY pte4;
  vmem_alloc_page(&pte4); fb_print("\n");
  fb_printx((u_int)pte4);
  PT_ENTRY pte5;
  vmem_alloc_page(&pte5); fb_print("\n");
  fb_printx((u_int)pte5);
  
  fb_print("\nCPU is going to halt. See you...\n");
}
