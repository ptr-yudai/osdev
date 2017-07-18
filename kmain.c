#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./sys/screen.h"
#include "./sys/forensics.h"
#include "./hal/hal.h"
#include "./fs/mbr.h"

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
  if (mbr->pTable1.type == MBR_PTYPE_NTFS)
    fb_print("Partition Table 1 is NTFS\n");
  if (mbr->pTable2.type == MBR_PTYPE_NTFS)
    fb_print("Partition Table 2 is NTFS\n");
  if (mbr->pTable3.type == MBR_PTYPE_NTFS)
    fb_print("Partition Table 3 is NTFS\n");
  if (mbr->pTable4.type == MBR_PTYPE_NTFS)
    fb_print("Partition Table 4 is NTFS\n");
  
  fb_print("\n[DEBUG] CPU is going to halt. See you...\n");
}
