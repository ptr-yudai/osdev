#include "ntfs_scanner.h"

/*
 * fls - 指定ディレクトリ下のファイル一覧を取得する
 *
 * @param mftSector MFTの開始セクタ番号
 */
void ntfs_fls(u_int mftSector)
{
  char* filename = (char*)malloc(1);    // ファイル名
  NTFS_MFT *mft;                        // MFTレコード
  // MFTレコードを取得
  mft = ntfs_mft(mftSector);
  // 不要な領域を解放
  free(mft, ntfs_info.sectorsPerRecord);  // [TODO] sectorsPerRecordではない
  free(filename, 1);
}

/*
 * NTFSの調査開始
 */
void ntfs_investigate(u_int mftSector)
// 今は決め打ちでルートディレクトリを選択
{
  char *filename = (char*)malloc(1);
  NTFS_MFT *mft;
  NTFS_ATTR_HEADER_R *mft_iroot;
  NTFS_ATTR_HEADER_NR *mft_iallc;
  NTFS_ENTRY_INDXROOT *attr_iroot;
  NTFS_INODE_I30_HEADER *inode;
  NTFS_RUNLIST *runlist = NULL;
  NTFS_RECORD_INDEX *rec_index;
  NTFS_ENTRY_FILENAME *rec_filename;
  /*
  NTFS_ATTR_HEADER_NR *entry_mft;
  // MFTレコードを取得
  mft = ntfs_mft(mftSector + ntfs_info.sectorsPerRecord * 0);
  entry_mft = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA);
  runlist = ntfs_parse_runlist(entry_mft);
  if (runlist == NULL) {
    fb_print("[WARNING] Invalid datarun!\n");
    return;
  }
  NTFS_MFT *data = (NTFS_MFT*)ntfs_find_index(runlist, 0);
  if (data == NULL) {
    fb_print("Invalid data!\n");
    return;
  }
  fb_printf("%s\n", data->signature);
  
  return;
  ////////// [DEBUG] ここまで
  */

  // FILEレコードを取得
  mft = ntfs_mft(mftSector + ntfs_info.sectorsPerRecord * 5);

  /*----- INDEX_ROOTを確認 -----*/
  // INDEX_ROOTを取得
  mft_iroot = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXROOT);
  // ストリーム名を取得
  memcpy((void*)filename,
	 (void*)((char*)mft_iroot + mft_iroot->nameOffset),
	 mft_iroot->nameLength * 2);
  unicode2ascii(filename, mft_iroot->nameLength);
  fb_printf("Stream name is \"%s\"\n", filename);
  attr_iroot = (NTFS_ENTRY_INDXROOT*)((char*)mft_iroot + mft_iroot->contentOffset);
  // INODE_I30を確認
  inode = (NTFS_INODE_I30_HEADER*)((char*)attr_iroot + 0x10 + attr_iroot->inode.inodeOffset);
  if (inode->flags != NTFS_MFT_ENTRY_FLAGS_SMALLINDX) {
    /*----- INDEX_ALLOCATIONを確認 -----*/
    // リストがINDEX_ALLOCに存在する(必ずNon-resident)
    mft_iallc = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXALLC);
    // datarunを取得
    runlist = ntfs_parse_runlist(mft_iallc);
    if (runlist == NULL) {
      fb_print("[WARNING] Invalid datarun!\n");
      return;
    }
    rec_index = ntfs_find_index(runlist, 0);

    /*----- INDEX_RECORDのファイル一覧を取得 -----*/
    inode = (NTFS_INODE_I30_HEADER*)((char*)rec_index + 0x18 + rec_index->inode.inodeOffset);
    // [TODO] inodeが破損していた場合に対応すること
    fb_print("----- FILES ON C:\\ -----\n");
    while(inode->flags != NTFS_MFT_INODE_FLAGS_TERMINATOR) {
      // ファイル情報を取得
      rec_filename = (NTFS_ENTRY_FILENAME*)((char*)inode + 0x10);
      memcpy(filename,
	     (void*)((char*)rec_filename + sizeof(NTFS_ENTRY_FILENAME)),
	     rec_filename->nameLength * 2);
      unicode2ascii(filename, rec_filename->nameLength);
      u_int64 unixtime = ts_file2unix(rec_filename->tsCreated);
      DATETIME datetime;
      ts_unix2date(unixtime, &datetime);
      fb_printf("%s: Created at %d/%d/%d %d:%d:%d FLAG:%x\n",
		filename,
		datetime.year, datetime.month, datetime.day,
		datetime.hour, datetime.minute, datetime.second,
		inode->flags);
      // 次のinodeへ
      inode = (NTFS_INODE_I30_HEADER*)((char*)inode + inode->length);
    }

    free(rec_index, runlist->length);// [TODO] 正しくfree
  } else {
    // [TODO]実装
    fb_print("[DEBUG] INDEX is resident! Implement it!\n");
  }
  
  // free
  free(mft, ntfs_info.sectorsPerRecord);
  free(mft_iroot, 1);
  free(mft_iallc, 1);
  free(runlist, 1);
  free(filename, 1);
}
/*
{ 
  int i, n;
  char *filename = (char*)malloc(1);
  NTFS_MFT *file_mft;
  NTFS_ATTR_HEADER_R *mft_filename;
  NTFS_ENTRY_FILENAME *attr_filename;
  
  for(i = 0; i < 12; i++) {
    // MFTエントリを読み込み
    file_mft = ntfs_mft(mftSector + ntfs_info.sectorsPerRecord * i);
    switch(file_mft->flags) {
    case 0x00: fb_print("[DEL    ] "); break;
    case 0x01: fb_print("[       ] "); break;
    case 0x02: fb_print("[DEL DIR] "); break;
    case 0x03: fb_print("[    DIR] "); break;
    default:   fb_print("[BROKEN!] "); break;
    }
    // FILENAME属性を探索
    mft_filename = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(file_mft, NTFS_MFT_ATTRIBUTE_FILENAME);
    // FILENAME属性本体を読み込み
    attr_filename = (NTFS_ENTRY_FILENAME*)((char*)mft_filename + mft_filename->contentOffset);
    // ファイル名を取得
    memcpy(filename,
	   (void*)((char*)attr_filename + sizeof(NTFS_ENTRY_FILENAME)),
	   attr_filename->nameLength * 2);
    unicode2ascii(filename, attr_filename->nameLength);
    fb_printf("%d --> %s\n", i, filename);
    // 解放
    free(file_mft, ntfs_info.sectorsPerRecord);
    free(mft_filename, 1);
    free(attr_filename, 1);
  }

  n = -1;
  while(n < 0 || n > 12) {
    fb_print("Enter the entry number you want to investigate: ");
    if (kb_getnumber(&n) == 0) {
      fb_print("[ERROR] Invalid number!\n");
      n = -1;
      continue;
    }
  }
  fb_printf("%d is selected.\n", n);

  file_mft = ntfs_mft(mftSector + ntfs_info.sectorsPerRecord * n);
  if (file_mft->flags & NTFS_FILE_FLAG_DIRECTORY) {
    fb_print("It is a directory.\n");
  } else {
    fb_print("It is a file.\n");
  }
  free(file_mft, ntfs_info.sectorsPerRecord);
  
  // 解放
  free(filename, 1);
}
*/


/*
 * mmls - パーティションレイアウトを表示する
 *
 * @return MFTの開始セクタ番号
 */
u_int ntfs_mmls(void)
{
  MBR* mbr;
  NTFS_BS* bootsector;
  int num;
  // MBRを割り当てる
  mbr = mbr_load();
  // パーティションテーブルの選択
  //// [TODO] ファイルシステムの種類が違う場合はその旨を表示すること
  fb_print("\n- Which partition do you want to investigate?\n");
  fb_print("# No. # START      # SIZE       #\n");
  fb_print("#-----#------------#------------#\n");
  //// [TODO] for文に直すこと
  // pTable1
  fb_print("# 1   # "); fb_printx(mbr->pTable1.lbaFirst);
  fb_print(" # "); fb_printx(mbr->pTable1.lbaCount); fb_print(" # ");
  if (mbr->pTable1.bootflag != 0x80) { fb_print("<-- Not in use or broken"); }
  // pTable2
  fb_print("\n# 2   # "); fb_printx(mbr->pTable2.lbaFirst);
  fb_print(" # "); fb_printx(mbr->pTable2.lbaCount); fb_print(" # ");
  if (mbr->pTable2.bootflag != 0x80) { fb_print("<-- Not in use or broken"); }
  // pTable3
  fb_print("\n# 3   # "); fb_printx(mbr->pTable3.lbaFirst);
  fb_print(" # "); fb_printx(mbr->pTable3.lbaCount); fb_print(" # ");
  if (mbr->pTable3.bootflag != 0x80) { fb_print("<-- Not in use or broken"); }
  // pTable4
  fb_print("\n# 4   # "); fb_printx(mbr->pTable4.lbaFirst);
  fb_print(" # "); fb_printx(mbr->pTable4.lbaCount); fb_print(" # ");
  if (mbr->pTable4.bootflag != 0x80) { fb_print("<-- Not in use or broken"); }
  //// [TODO] 実際には対話形式でパーティション番号を尋ねる
  num = 1;
  bootsector = ntfs_bootsector(mbr, num);
  // 不要な領域の解放
  free(mbr, 1);
  free(bootsector, 1);
  // 終了
  return bootsector->mftCluster * ntfs_info.sectorsPerCluster;
}
