#include "shell.h"

/*
 * コマンド受付
 */
void k_shell(void)
{
  char cmd[64];
  
  // 初期化
  sh_info.mftref = 0;
  sh_info.mftSector = 0;

  // コマンド受付
  while(1) {
    scr_switch(1);
    fb_printf("[%d]# ", sh_info.mftref);
    kb_getline(cmd);

    // 結果用画面を初期化
    scr_switch(2);
    fb_clrscr();

    // mmls - パーティションの指定
    if (strncmp(cmd, "mmls", 5) == 0) {
      sh_info.mftref = ntfs_mmls();
      sh_info.mftSector = sh_info.mftref;
    }
    // fls - ファイル一覧
    if (strncmp(cmd, "fls", 4) == 0) {
      if (sh_info.mftSector == 0) {
	fb_debug("$MFT Sector is required. (not initialized)\n", ER_CATION);
      } else {
	ntfs_fls(sh_info.mftSector);
      }
    }
  }
}
