#include "ntfs_scanner.h"

/**
 * parselog - $LogFileを解析する
 *
 * @param mftSector $MFTの開始番号
 */
void ntfs_parselog(u_int mftSector)
{
  char c;
  int i;
  int                       func;        // カーヴィングの機能
  char *b_clue = (char*)malloc(1);       // カーヴィングで使うバイトデータ
  int                       i_clue;      // カーヴィングで使う数値データ
  NTFS_MFT                 *mft;         // MFTレコード
  NTFS_ATTR_HEADER_R       *mft_data;    // DATA属性のヘッダ
  NTFS_RUNLIST             *runlist;     // データのrunlist
  char                     *log;         // データ実体
  //NTFS_RESTART_PAGE_HEADER *log_header;  // LogFileのヘッダ
  //NTFS_RESTART_AREA        *log_area;    // Restart Area

  memset(b_clue, 0, MEMORY_BLOCK_SIZE);

  // Sorry Message
  fb_print("Sorry, but this function is not complete "
	   "since no official specification of $LogFile has been released.\n"
	   "Although, you can do a file carving within $LogFile, "
	   "which might help you with finding an evidence.\n"
	   "Would you like to do file carving? [Y/n]: ");
  c = kb_getc();
  if (c != 'Y' && c != 'y') {
    goto parselog_return;
  }

  // 参考にする情報を選択
  // [TODO] もっと追加
  fb_print("\nSelect which clue to use in searching a file?\n"
	   "1) Filename\n"
	   "2) Filesize\n"
	   ">> ");
  func = -1;
  while(0 < func || func < 3) {
    if (kb_getnumber(&func)) {
      break;
    } else {
      func = -1;
    }
    fb_debug("Invalid function specified.\n", ER_CATION);
  }
  // 参考にする情報を入力
  switch(func) {
  case NTFS_CARVING_FILENAME:
    fb_print("Filename: ");
    kb_getline(b_clue);
    break;
  case NTFS_CARVING_FILESIZE:
    i_clue = -1;
    while(i_clue > 0) {
      fb_print("Filesize: ");
      if (kb_getnumber(&i_clue)) break;
      fb_debug("Invalid data specified.\n", ER_CATION);
    }
    break;
  default:
    fb_debug("How did you reach here!?\n", ER_WARNING);
    goto parselog_return;
  }
  fb_debug("Seeking DATA of $LogFile...\n", ER_INFO);
  fb_print("Just a moment......\n");

  // 参照番号に一致したMFTを取得
  mft = ntfs_getrecord(mftSector, NTFS_MFT_INODE_LOGFILE);
  if (mft == NULL) {
    scr_switch(1);
    fb_debug("$LogFile does not exist.\n", ER_CATION);
    goto parselog_return;
  }
  // 不正なMFT
  if (!((mft->signature[0] == 'F') &
	(mft->signature[1] == 'I') &
	(mft->signature[2] == 'L') &
	(mft->signature[3] == 'E'))) {
    mft = NULL;
    fb_debug("Invalid MFT Record!\n", ER_CATION);
    goto parselog_return;
  }

  // [DEBUG] DATA属性を取得
  mft_data = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA, 0);
  // Resident
  if (mft_data->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    fb_debug("Resident $LogFile is not supported!\n", ER_FATAL);
  } else {
    runlist = ntfs_parse_runlist((NTFS_ATTR_HEADER_NR*)mft_data);
    log = (char*)ntfs_find_data(runlist, 0);
    // RESTART_PAGE_HEADERを取得
    //log_header = (NTFS_RESTART_PAGE_HEADER*)log;
    // RESTART_AREAを取得
    //log_area = (NTFS_RESTART_AREA*)((char*)log_header + sizeof(NTFS_RESTART_PAGE_HEADER) + log_header->restartOffset);
    fb_debug("Found RESTART_AREA. Now carving begins...\n", ER_INFO);
    
    // カービング
    char* page_header = (char*)log + 0x2000;
    for(i = 0; ; i++) {
      page_header += 0x1000;
      if (strncmp(page_header, "RCRD", 4) != 0) break;
      ntfs_carving((char*)log + 0x2000, 0x1000, func, b_clue, i_clue);
    }

    free(runlist, 1);
  }
  
  // 不要な領域を解放
 parselog_return:
  free(b_clue, 1);
  free(mft, ntfs_info.sectorsPerRecord);
}
