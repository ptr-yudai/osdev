#include "ntfs_scanner.h"

/*
 * icat - 特定のファイル内容を取得する
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Reference Number (ID)
 */
void ntfs_icat(u_int mftSector, u_int64 mftref)
{
  char* filename = (char*)malloc(1);     // ファイル名
  NTFS_MFT              *tmft;           // リスト中のレコード

  // 参照番号に一致したMFTを取得
  tmft = ntfs_getrecord(mftSector, mftref);
  if (tmft == NULL) {
    scr_switch(1);
    fb_debug("Specified file or directory not found.\n", ER_CATION);
    goto icat_return;
  }

  // [DEBUG] 仮にFILENAMEを取得してみる
  NTFS_ATTR_HEADER_R* mft_filename;
  NTFS_ENTRY_FILENAME* attr_filename;
  mft_filename = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_FILENAME);
  attr_filename = (NTFS_ENTRY_FILENAME*)((char*)mft_filename + mft_filename->contentOffset);
  memcpy(filename,
	 (void*)((char*)attr_filename + sizeof(NTFS_ENTRY_FILENAME)),
	 attr_filename->nameLength * 2);
  unicode2ascii(filename, attr_filename->nameLength);
  fb_printf("FOUND: %s\n", filename);
  
  // 不要な領域を解放
 icat_return:
  free(tmft, ntfs_info.sectorsPerRecord);
  free(filename, 1);
}

/*
 * ntfs_cd - 指定ディレクトリに移動
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Reference Number (ID)
 */
void ntfs_cd(u_int mftSector, u_int mftref)
{
  NTFS_MFT *mft;

  // レコードを取得
  mft = ntfs_getrecord(mftSector, mftref);
  if (mft == NULL) {
    scr_switch(1);
    fb_debug("Specified file or directory not found.\n", ER_CATION);
    return;
  }

  // [TODO] ディレクトリであるかを判定
  mftref = mftref;
}

/*
 * ntfs_getrecord - 指定ファイルのMFTレコードを取得する
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Reference Number (ID)
 */
NTFS_MFT* ntfs_getrecord(u_int mftSector, u_int mftref)
{
  NTFS_MFT              *mft;            // $MFTレコード
  NTFS_ATTR_HEADER_NR   *mft_data;       // $MFTのDATA属性
  NTFS_RUNLIST          *runlist = NULL; // datarun
  void                  *mft_list;       // リストの先頭
  NTFS_MFT              *tmft = NULL;    // リスト中のレコード

  // $MFTのレコードを取得
  mft = ntfs_mft(mftSector);
  if (!((mft->signature[0] == 'F') &
	(mft->signature[1] == 'I') &
	(mft->signature[2] == 'L') &
	(mft->signature[3] == 'E'))) {
    fb_debug("Invalid MFT Record!\n", ER_WARNING);
    goto getref_return;
  }

  // DATA属性を取得(ほぼ間違いなくnon-resident)
  mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA);
  // [TODO] residentの実装(優先度低)
  if (mft_data->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    fb_debug("Resident $MFT is not supported!\n", ER_FATAL);
    goto getref_return;
  }
  // datarunを取得
  runlist = ntfs_parse_runlist(mft_data);
  if (runlist == NULL) {
    scr_switch(0);
    fb_debug("Invalid datarun!\n", ER_WARNING);
    goto getref_return;
  }
  // 0番目のrunlistを取得
  // [TODO!] 全部のdatarunを調べること
  mft_list = ntfs_find_data(runlist, 0);

  // 対象のIDに到達するまで調べる
  while(1) {
    // 対象のレコードを取得する
    tmft = mft_list;
    // [TODO] datarunのlengthまでを調べるのが正攻法
    if (!((tmft->signature[0] == 'F') &
	  (tmft->signature[1] == 'I') &
	  (tmft->signature[2] == 'L') &
	  (tmft->signature[3] == 'E'))) {
      // [TODO] 次のdatarunへ移行
      tmft = NULL;
      goto getref_return;
    }
    
    // IDをチェック
    // [TODO] このチェック方法は怪しい？
    if (tmft->MFTRecNumber == (u_int)mftref) {
      tmft = (NTFS_MFT*)malloc(ntfs_info.sectorsPerRecord);
      memcpy(tmft, mft_list, ntfs_info.sectorsPerRecord * ntfs_info.bytesPerSector);
      break;
    }
    
    mft_list += ntfs_info.bytesPerSector * ntfs_info.sectorsPerRecord;
  }

  free(mft_list, runlist->length);
  // [TODO] loop

 getref_return:
  free(runlist, 1);
  free(mft, ntfs_info.sectorsPerRecord);
  return tmft;
}

/*
 * fls - 指定ディレクトリ下のファイル一覧を取得する
 *
 * @param mftSector $MFTの開始セクタ番号
 */
void ntfs_fls(u_int mftSector)
{
  char* filename = (char*)malloc(1);     // ファイル名
  NTFS_MFT              *mft;            // MFTレコード
  NTFS_ATTR_HEADER_R    *mft_iroot;      // INDEX_ROOT属性のヘッダ
  NTFS_ENTRY_INDXROOT   *entry_iroot;    // INDEX_ROOTのエントリ本体
  NTFS_INODE_I30_HEADER *inode;          // INODE_I30
  NTFS_ATTR_HEADER_NR   *mft_iallc;      // INDEX_ALLOCATION属性のヘッダ
  NTFS_RUNLIST          *runlist = NULL; // datarun
  NTFS_RECORD_INDEX     *rec_index;      // INDEXレコード
  NTFS_ENTRY_FILENAME   *rec_filename;   // INDEXレコードのFILENAME

  // .(dot)のレコードを取得
  u_int rootSector = mftSector
    + ntfs_info.sectorsPerRecord * NTFS_MFT_INODE_ROOTDIR;
  mft = ntfs_mft(rootSector);
  if (!((mft->signature[0] == 'F') &
	(mft->signature[1] == 'I') &
	(mft->signature[2] == 'L') &
	(mft->signature[3] == 'E'))) {
    fb_debug("Invalid MFT Record!\n", ER_WARNING);
    goto fls_return;
  }

  // INDEX_ROOT属性を取得(常にresident)
  mft_iroot = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXROOT);
  // INDEX_ROOT属性のエントリーを取得
  entry_iroot = (NTFS_ENTRY_INDXROOT*)((char*)mft_iroot + mft_iroot->contentOffset);

  // INDEX_ROOT属性のINODE_I30を確認
  inode = (NTFS_INODE_I30_HEADER*)((char*)entry_iroot + 0x10 + entry_iroot->inode.inodeOffset);
  if (inode->flags != NTFS_MFT_ENTRY_FLAGS_SMALLINDX) {
    //// SMALLならINDEX_ALLOCATIONを調べる
    // リストがINDEX_ALLOCに存在する(必ずNon-resident)
    mft_iallc = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXALLC);
    // datarunを取得
    runlist = ntfs_parse_runlist(mft_iallc);
    if (runlist == NULL) {
      scr_switch(0);
      fb_debug("Invalid datarun!\n", ER_WARNING);
      goto fls_return;
    }
    // 0番目のrunlistを取得
    rec_index = (NTFS_RECORD_INDEX*)ntfs_find_data(runlist, 0);

    // INDEX_RECORDでファイル一覧を取得
    // [TODO] inodeが破損していた場合に対応すること
    inode = (NTFS_INODE_I30_HEADER*)((char*)rec_index + 0x18 + rec_index->inode.inodeOffset);

    // 終端まで調べる
    while((inode->flags & NTFS_MFT_INODE_FLAGS_TERMINATOR) == 0) {
      // ファイル情報を取得
      rec_filename = (NTFS_ENTRY_FILENAME*)((char*)inode + 0x10);
      memcpy(filename,
	     (void*)((char*)rec_filename + sizeof(NTFS_ENTRY_FILENAME)),
	     rec_filename->nameLength * 2);
      unicode2ascii(filename, rec_filename->nameLength);
      // ファイル種別
      if (rec_filename->flags & NTFS_MFT_ENTRY_FLAGS_DIRECTORY) {
	fb_print("[DIR]  ID:");
      } else {
	fb_print("[FILE] ID:");
      }
      fb_printx(inode->mftref & (u_int)0x00FFFFFFffffffff);
      fb_printf("  %s\n", filename);
      // 次のinodeへ
      inode = (NTFS_INODE_I30_HEADER*)((char*)inode + inode->length);
    }

    free(rec_index, runlist->length);// [TODO] 正しくfree
    free(mft_iallc, 1);
  } else {
    // [TODO]実装
    fb_debug("INDEX is resident! Implement it!\n", ER_FATAL);
  }

  // 不要な領域を解放
 fls_return:
  free(mft, ntfs_info.sectorsPerRecord);  // [TODO] sectorsPerRecordではない
  free(mft_iroot, 1);
  free(runlist, 1);
  free(filename, 1);
}

/*
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
    fb_print(" # "); fb_printx(ptList[i].lbaCount); fb_print(" # ");
    if (ptList[i].bootflag != 0x80) { fb_print("<-- Not in use or broken\n"); }
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
  // 不要な領域の解放
  free(mbr, 1);
  free(bootsector, 1);
  // 終了
  return bootsector->mftCluster * ntfs_info.sectorsPerCluster;
}
