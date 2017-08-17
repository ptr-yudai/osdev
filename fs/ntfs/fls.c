#include "ntfs_scanner.h"

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
	  fb_print("  SIZE: ");
	  fb_printx(entry_filename->physicalSize);
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
	fb_print("  SIZE:");
	fb_printx(rec_filename->physicalSize);
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
	    fb_printf("                                       %s:%s\n", filename, adsname);
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
