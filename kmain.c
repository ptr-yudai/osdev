#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./sys/screen.h"
#include "./hal/hal.h"
#include "./hal/mem.h"

/*
 * カーネルメイン
 */
void kmain(multiboot_info_t* mbd, u_int magic)
{
  // 割り込みを設定
  hal_init(mbd);

  // 画面を初期化
  screen_init();

  // 各種情報を表示
  /*
  fb_print("Magic Number: ");
  fb_printx(magic); fb_print("\n");
  fb_print("Memory Size: ");
  fb_printx(mbd->mem_upper / 1024); fb_print(" MB\n");
  //*/
  
  fb_print("\nCPU is going to halt. See you...\n");
}
