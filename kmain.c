#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./sys/screen.h"
#include "./hal/hal.h"
#include "./hal/vmem.h"
#include "./hal/ide.h"

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

  char buf[1024];
  ata_read(buf, 4, 3);
  
  fb_print("\nCPU is going to halt. See you...\n");
}
