#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./sys/screen.h"
#include "./sys/forensics.h"
#include "./hal/hal.h"

#include "./hal/ide.h"
#include "./fs/mbr.h"
#include "./fs/ntfs.h"

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
  fb_print("[INFO] Magic Number: ");
  fb_printx(magic); fb_print("\n");
  fb_print("[INFO] Memory Size: ");
  fb_printx(mbd->mem_upper / 1024); fb_print(" MB\n");
  //*/

  // MBRをロード
  MBR* mbr = mbr_load();
  // とりあえずpTable1を調査
  NTFS_BOOTSECTOR* ntfs_bs = ntfs_bootsector(mbr);
  ntfs_bs = ntfs_bs;
  
  fb_print("\n[DEBUG] CPU is going to halt. See you...\n");
}
