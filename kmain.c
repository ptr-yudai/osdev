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
  fb_setpos(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
  fb_clrscr();
  // 割り込みを設定
  hal_init(mbd);
  // 画面管理を初期化
  screen_init();

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
  NTFS_BS* bootsector = ntfs_bootsector(mbr);
  NTFS_MFT* mft = ntfs_mft(bootsector->mftCluster);
  //NTFS_ATTR_HEADER_NR *mft_data1 = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA);
  NTFS_ATTR_HEADER_R *mft_data = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_FILENAME);
  NTFS_ENTRY_FILENAME *entry_filename = (NTFS_ENTRY_FILENAME*)((u_int)mft_data + mft_data->contentOffset);

  /* 動かず
  u_int64 unixtime = ts_file2unix(entry_filename->tsAccessed);
  DATETIME date;
  ts_unix2date(unixtime, &date);
  fb_printx(date.year);
  */

  char *filename = (char*)malloc(1);
  memcpy(filename, (void*)((u_int)entry_filename + sizeof(NTFS_ENTRY_FILENAME)), entry_filename->nameLength * 2);
  fb_printb(filename, entry_filename->nameLength * 2);
  free(filename, 1);
  
  fb_print("\n[DEBUG] CPU is going to halt. See you...\n");
  
  free(mbr, 1);
}
