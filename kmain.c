#include "./include/multiboot.h"
#include "./include/io.h"
#include "./include/time.h"
#include "./include/util.h"
#include "./sys/screen.h"
#include "./hal/hal.h"

#include "./hal/ide.h"
#include "./fs/mbr.h"
#include "./fs/ntfs.h"
#include "./fs/ntfs_scanner.h"

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
  fb_printf("[INFO] Magic Number: 0x%x\n", magic);
  fb_printf("[INFO] Memory Size: %d MB\n", mbd->mem_upper / 1024);

  // MBRをロード
  MBR* mbr = mbr_load();
  // とりあえずpTable1を調査
  NTFS_BS* bootsector = ntfs_bootsector(mbr);
  // 固定MFTエントリを読む
  ntfs_investigate(bootsector->mftCluster * ntfs_info.sectorsPerCluster);
  //NTFS_MFT* mft = ntfs_mft(bootsector->mftCluster);
  /*
  NTFS_ATTR_HEADER_NR *mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA);
  if (mft_data->formCode == 0) {
    // Resident
    NTFS_ATTR_HEADER_R *data = (NTFS_ATTR_HEADER_R*)mft_data;
    fb_printf("Filesize = %d\n", data->contentLength);
  } else {
    // Non-Resident
    NTFS_ATTR_HEADER_NR *data = (NTFS_ATTR_HEADER_NR*)mft_data;
    fb_printf("Filesize = %dbyte\n", data->contentSize);
    // datarunを取得
    NTFS_RUNLIST *runlist = NULL;
    runlist = ntfs_parse_runlist(data);
    fb_printf("runlist: 0x%x\n", runlist);
    if (runlist == NULL) {
      fb_print("[WARNING] Invalid datarun!");
    } else {
      fb_print("[INFO] It works!");
    }
    // [TODO]データ取得
  }
  */
  
  fb_print("\n[DEBUG] CPU is going to halt. See you...\n");
  
  free(mbr, 1);
}
