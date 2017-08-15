#include "ntfs_scanner.h"

/**
 * icat - 特定のファイル内容を取得する
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Reference Number (ID)
 */
void ntfs_icat(u_int mftSector, u_int64 mftref)
{
  char* filename = (char*)malloc(1);     // ファイル名
  NTFS_MFT              *tmft;           // リスト中のレコード
  NTFS_ATTR_HEADER_R    *mft_data;       // DATA属性のヘッダ
  NTFS_RUNLIST          *runlist;        // データのrunlist
  char                  *data;           // データ実体

  // 参照番号に一致したMFTを取得
  tmft = ntfs_getrecord(mftSector, mftref);
  if (tmft == NULL) {
    scr_switch(1);
    fb_debug("Specified file or directory not found.\n", ER_CATION);
    goto icat_return;
  }

  // [DEBUG] DATA属性を取得
  mft_data = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_DATA, 0);
  // Resident
  if (mft_data->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    data = ((char*)mft_data + mft_data->contentOffset);
    fb_printb(data, 64);
    fb_print("\n");
  } else {
    runlist = ntfs_parse_runlist((NTFS_ATTR_HEADER_NR*)mft_data);
    // [TODO] 全部取得すること
    data = (char*)ntfs_find_data(runlist, 0);
    //fb_printf("%s\n", data);
    fb_printb(data, 64);
    fb_print("\n");
    free(runlist, 1);
  }
  
  // 不要な領域を解放
 icat_return:
  free(tmft, ntfs_info.sectorsPerRecord);
  free(filename, 1);
}

/**
 * ntfs_cd - 指定ディレクトリに移動
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Reference Number (ID)
 */
u_int ntfs_cd(u_int mftSector, u_int mftref)
{
  NTFS_MFT *mft;
  NTFS_ATTR_HEADER_R *mft_filename;
  NTFS_ENTRY_FILENAME* attr_filename;
  u_int nextref = ERROR;
  
  // レコードを取得
  mft = ntfs_getrecord(mftSector, mftref);
  if (mft == NULL) {
    scr_switch(1);
    fb_debug("Specified file or directory not found.\n", ER_CATION);
    goto cd_return;
  }

  // FILENAMEエントリを取得
  mft_filename = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_FILENAME, 0);
  if (mft_filename == NULL) {
    fb_debug("Filename attribute not found.\n", ER_CATION);
    goto cd_return;
  }
  attr_filename = (NTFS_ENTRY_FILENAME*)((char*)mft_filename + mft_filename->contentOffset);
  
  // ディレクトリかを判別
  if ((attr_filename->flags & NTFS_MFT_ENTRY_FLAGS_DIRECTORY) == 0) {
    fb_debug("Selected file is not a directory.\n", ER_CATION);
    goto cd_return;
  }

  nextref = mftref;

  // 不要な領域を解放
 cd_return:
  free(mft, ntfs_info.sectorsPerRecord);
  return nextref;
}

/**
 * ntfs_getrecord - 指定ファイルのMFTレコードを取得する
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Reference Number (ID)
 */
NTFS_MFT* ntfs_getrecord(u_int mftSector, u_int mftref)
{
  int i, j;
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
  mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA, 0);
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

  // runlistを探索
  for(i = 0; ; i++) {
    NTFS_RUNLIST *trun;
    trun = ntfs_extract_runlist(runlist, i);
    // RUNLISTからMFTに到達する
    mft_list = ntfs_find_data(runlist, i);
    if (mft_list == NULL) {
      break;
    }

    // 対象のIDに到達するまで調べる
    // [TODO] 無限ループの可能性あり！
    for(j = 0; j < 32; j++) {
      // 対象のレコードを取得する
      tmft = mft_list;
      // [TODO] datarunのlengthまでを調べるのが正攻法
      /*
      if (!((tmft->signature[0] == 'F') &
	    (tmft->signature[1] == 'I') &
	    (tmft->signature[2] == 'L') &
	    (tmft->signature[3] == 'E'))) {
	// 次のdatarunへ移行
	tmft = NULL;
	break;
      }
      */
      
      /*
      char* filename = malloc(1);
      NTFS_ENTRY_FILENAME* rec_filename;
      NTFS_ATTR_HEADER_R *attr_filename = ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_FILENAME, 0);
      rec_filename = (NTFS_ENTRY_FILENAME*)((char*)attr_filename + attr_filename->contentOffset);
      memcpy(filename,
	     (void*)((char*)rec_filename + sizeof(NTFS_ENTRY_FILENAME)),
	     rec_filename->nameLength * 2);
      unicode2ascii(filename, rec_filename->nameLength);
      fb_printf("%s; ", filename);
      free(filename, 1);
      */

      // IDをチェック
      // [TODO] このチェック方法は怪しい？
      if (tmft->MFTRecNumber == (u_int)mftref) {
	tmft = (NTFS_MFT*)malloc(ntfs_info.sectorsPerRecord);
	memcpy(tmft, mft_list, ntfs_info.sectorsPerRecord * ntfs_info.bytesPerSector);
	break;
      }
      
      mft_list += ntfs_info.bytesPerSector * ntfs_info.sectorsPerRecord;
    }

    free(mft_list, trun->length);
    if (tmft->MFTRecNumber == (u_int)mftref) {
      break;
    }
  }

 getref_return:
  free(runlist, 1);
  free(mft, ntfs_info.sectorsPerRecord);
  return tmft;
}

/**
 * fls - 指定ディレクトリ下のファイル一覧を取得する（削除済み）
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    解析するディレクトリID
 */
void ntfs_fls(u_int mftSector, u_int mftref)
{
  int i, j;
  char *filename = (char*)malloc(1);     // ファイル名
  NTFS_MFT              *mft;            // $MFTレコード
  NTFS_ATTR_HEADER_NR   *mft_data;       // $MFTのDATA属性
  NTFS_RUNLIST          *runlist = NULL; // datarun
  void                  *mft_list;       // リストの先頭
  NTFS_MFT              *tmft = NULL;    // リスト中のレコード
  NTFS_ATTR_HEADER_R    *attr_filename;  // FILENAME属性
  NTFS_ENTRY_FILENAME   *entry_filename; // FILENAMEエントリ

  // $MFTのレコードを取得
  mft = ntfs_mft(mftSector);
  if (!((mft->signature[0] == 'F') &
	(mft->signature[1] == 'I') &
	(mft->signature[2] == 'L') &
	(mft->signature[3] == 'E'))) {
    fb_debug("Invalid MFT Record!\n", ER_WARNING);
    goto fls_return;
  }

  // DATA属性を取得(ほぼ間違いなくnon-resident)
  mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA, 0);
  // [TODO] residentの実装(優先度低)
  if (mft_data->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    fb_debug("Resident $MFT is not supported!\n", ER_FATAL);
    goto fls_return;
  }
  // datarunを取得
  runlist = ntfs_parse_runlist(mft_data);
  if (runlist == NULL) {
    scr_switch(0);
    fb_debug("Invalid datarun!\n", ER_WARNING);
    goto fls_return;
  }

  // runlistを探索
  for(i = 0; ; i++) {
    NTFS_RUNLIST *trun;
    trun = ntfs_extract_runlist(runlist, i);
    // RUNLISTからMFTに到達する
    mft_list = ntfs_find_data(runlist, i);
    if (mft_list == NULL) {
      break;
    }

    // 対象のIDに到達するまで調べる
    // [TODO] ちゃんと書け！
    for(j = 0; j < 100; j++) {
      // 対象のレコードを取得する
      tmft = mft_list;
      
      // FILENAME属性を取得
      attr_filename = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_FILENAME, 0);
      // FILENAMEを取得
      entry_filename = (NTFS_ENTRY_FILENAME*)((char*)attr_filename + attr_filename->contentOffset);

      // 親ディレクトリIDをチェック
      if ((u_int)entry_filename->parentDir == mftref) {
	if ((tmft->flags & NTFS_FILE_FLAG_FILE_IN_USE) == 0) {
	  // ファイル名を取得
	  memcpy(filename,
		 (void*)((char*)entry_filename + sizeof(NTFS_ENTRY_FILENAME)),
		 entry_filename->nameLength * 2);
	  unicode2ascii(filename, entry_filename->nameLength);
	  // ファイル種別
	  if (entry_filename->flags & NTFS_MFT_ENTRY_FLAGS_DIRECTORY) {
	    fb_print("*DELETED* [DIR]  ID:");
	  } else {
	    fb_print("*DELETED* [FILE] ID:");
	  }
	  fb_printx(tmft->MFTRecNumber);
	  fb_printf("  %s\n", filename);
	}
      }
      
      mft_list += ntfs_info.bytesPerSector * ntfs_info.sectorsPerRecord;

      free(attr_filename, 1);
    }

    free(mft_list, trun->length);
  }

 fls_return:
  free(runlist, 1);
  free(mft, ntfs_info.sectorsPerRecord);
  free(filename, 1);
}

/**
 * ls - 指定ディレクトリ下のファイル一覧を取得する（削除なし）
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    解析するディレクトリID
 */
void ntfs_ls(u_int mftSector, u_int mftref)
{
  int i, j;
  char* filename = (char*)malloc(1);     // ファイル名
  char* adsname  = (char*)malloc(1);     // ADS名
  NTFS_MFT              *mft;            // MFTレコード
  NTFS_ATTR_HEADER_R    *mft_iroot;      // INDEX_ROOT属性のヘッダ
  NTFS_ENTRY_INDXROOT   *entry_iroot;    // INDEX_ROOTのエントリ本体
  NTFS_INODE_I30_HEADER *inode;          // INODE_I30
  NTFS_ATTR_HEADER_NR   *mft_iallc;      // INDEX_ALLOCATION属性のヘッダ
  NTFS_RUNLIST          *runlist = NULL; // datarun
  NTFS_RECORD_INDEX     *rec_index;      // INDEXレコード
  NTFS_ENTRY_FILENAME   *rec_filename;   // INDEXレコードのFILENAME
  NTFS_MFT              *tmft;           // ADS取得用のMFTレコード
  NTFS_ATTR_HEADER_R    *tmft_data;      //

  // .(dot)のレコードを取得
  mft = ntfs_getrecord(mftSector, mftref);
  if (!((mft->signature[0] == 'F') &
	(mft->signature[1] == 'I') &
	(mft->signature[2] == 'L') &
	(mft->signature[3] == 'E'))) {
    fb_debug("Invalid MFT Record!\n", ER_WARNING);
    goto ls_return;
  }

  // INDEX_ROOT属性を取得(常にresident)
  mft_iroot = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXROOT, 0);
  // INDEX_ROOT属性のエントリーを取得
  entry_iroot = (NTFS_ENTRY_INDXROOT*)((char*)mft_iroot + mft_iroot->contentOffset);

  // INDEX_ROOT属性のINODE_I30を確認
  inode = (NTFS_INODE_I30_HEADER*)((char*)entry_iroot + 0x10 + entry_iroot->inode.inodeOffset);
  if (inode->flags != NTFS_MFT_ENTRY_FLAGS_SMALLINDX) {
    //// SMALLならINDEX_ALLOCATIONを調べる
    // リストがINDEX_ALLOCに存在する(必ずNon-resident)
    mft_iallc = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXALLC, 0);
    // datarunを取得
    runlist = ntfs_parse_runlist(mft_iallc);
    if (runlist == NULL) {
      scr_switch(0);
      fb_debug("Invalid datarun!\n", ER_WARNING);
      goto ls_return;
    }
    for(i = 0; ; i++) {
      rec_index = (NTFS_RECORD_INDEX*)ntfs_find_data(runlist, i);
      if (rec_index == NULL) break;

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

	// 代替データストリームを探索
	tmft = ntfs_getrecord(mftSector, inode->mftref);
	for(j = 0; ; j++) {
	  tmft_data = ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_DATA, j);
	  if (tmft_data == NULL) break;
	  if (tmft_data->nameLength != 0) {
	    // ADS発見(filename変数を上書きして再利用しているので注意)
	    memcpy(adsname,
		   (void*)((char*)tmft_data + tmft_data->nameOffset),
		   tmft_data->nameLength * 2);
	    unicode2ascii(adsname, tmft_data->nameLength);
	    fb_printf("                      %s:%s\n", filename, adsname);
	  }
	}
	
	// 次のinodeへ
	inode = (NTFS_INODE_I30_HEADER*)((char*)inode + inode->length);

	free(tmft, ntfs_info.sectorsPerRecord);  // [TODO] sectorsPerRecordではない
      }

      free(rec_index, runlist->length);// [TODO] 正しくfree
      free(mft_iallc, 1);
    }
  } else {
    //// INDEX_ROOTを調べる
    // レコードの終端まで調べる
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

  }

  // 不要な領域を解放
 ls_return:
  free(mft, ntfs_info.sectorsPerRecord);  // [TODO] sectorsPerRecordではない
  free(mft_iroot, 1);
  free(runlist, 1);
  free(filename, 1);
  free(adsname, 1);
}

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
