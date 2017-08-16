#include "ntfs_scanner.h"

/**
 * mmls - パーティションレイアウトを表示する
 *
 * @return MFTの開始セクタ番号
 */
u_int ntfs_mmls(void)
{
  MBR* mbr;
  NTFS_BS* bootsector;
  int num = -1, i;
  pTable ptList[4];
  u_int mftCluster;

  // MBRを割り当てる
  mbr = mbr_load();
  ptList[0] = mbr->pTable1;
  ptList[1] = mbr->pTable2;
  ptList[2] = mbr->pTable3;
  ptList[3] = mbr->pTable4;
  // パーティションテーブルの選択
  //// [TODO] ファイルシステムの種類が違う場合はその旨を表示すること
  fb_print("\n- Which partition do you want to investigate?\n");
  fb_print("# No. # START      # SIZE       #\n");
  fb_print("#-----#------------#------------#\n");

  // パーティションテーブル一覧
  for(i = 0; i < 4; i++) {
    fb_printf("# %d   # ", i + 1); fb_printx(ptList[i].lbaFirst);
    fb_print(" # "); fb_printx(ptList[i].lbaCount);
    if (ptList[i].bootflag == 0x80) {
      fb_print(" # \n");
    } else {
      fb_print(" # <-- Not in use or broken\n");
    }
  }
  
  // パーティション番号を尋ねる
  while(num < 1 || num > 4) {
    fb_print(">> ");
    // 入力
    if (kb_getnumber(&num) == 0) {
      fb_print("[ERROR] Enter a valid number of partition table.\n");
      continue;
    }
    /*
    // 破損を通知
    if (ptList[num].bootflag != 0x80) {
    char c;
    fb_print("The partition is not used or corrupted.\n"
    "Are you sure you want to analyse this one? [Y/n] ");
    c = kb_getc();
    if (c != 'Y' && c != 'y') {
    num = -1;
    }
    }
    */
  }
  bootsector = ntfs_bootsector(mbr, num);
  if (bootsector == NULL) {
    free(mbr, 1);
    return 0;
  }
  mftCluster = bootsector->mftCluster;
  // 不要な領域の解放
  free(mbr, 1);
  free(bootsector, 1);
  // 終了
  return mftCluster * ntfs_info.sectorsPerCluster;
}
