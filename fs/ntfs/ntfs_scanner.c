#include "ntfs_scanner.h"

/**
 * ntfs_getpath - 指定ファイルまでのパスを取得する
 *
 * @param  mftSector $MFTの開始セクタ番号
 * @param  mftref    MFT Reference Number (ID)
 * @return ファイルの絶対パス
 */
char* ntfs_getpath(u_int mftSector, u_int mftref)
{
  int i;
  char *filepath = (char*)malloc(1);     // 最終的なパス
  char *filename = (char*)malloc(1);
  NTFS_MFT              *tmft;           // MFTレコード
  NTFS_ATTR_HEADER_R    *attr_filename;  // 属性のヘッダ
  NTFS_ENTRY_FILENAME   *entry_filename; // FILENAMEエントリ
  u_int filepath_len = 0;                // ファイルパス名の長さ
  u_int nextref = mftref;                // 次に参照するmftref
  u_int preref = -1;                     // 前に参照したmftref

  memset(filepath, 0, MEMORY_BLOCK_SIZE);
  memset(filename, 0, MEMORY_BLOCK_SIZE);

  while(1) {
    tmft = ntfs_getrecord(mftSector, nextref);
    if (tmft == NULL) {
      memcpy(filepath, "?OVERFLOW?", 11);
      goto getpath_return;
      break;
    }
    if (!((tmft->signature[0] == 'F') &
	  (tmft->signature[1] == 'I') &
	  (tmft->signature[2] == 'L') &
	  (tmft->signature[3] == 'E'))) {
      memcpy(filepath, "?INVALID?", 10);
      goto getpath_return;
      break;
    }

    // FILENAME属性を取得
    for(i = 0; ; i++) {
      attr_filename = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_FILENAME, i);
      if (attr_filename == NULL) {
	// 1つも見つからない場合は不明である
	if (i == 0) {
	  memcpy(filepath, "?UNNAMED?", 10);
	  goto getpath_return;
	}
	// 見つからない場合は前のものを使う
	attr_filename = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_FILENAME, i - 1);
	break;
      }
      entry_filename = (NTFS_ENTRY_FILENAME*)((char*)attr_filename + attr_filename->contentOffset);
      // DOS以外ならOK
      if (entry_filename->nameType != NTFS_MFT_ENTRY_NAMETYPE_DOS) break;
    }
    // ファイル名を取得
    memcpy(filename,
	   (void*)((char*)entry_filename + sizeof(NTFS_ENTRY_FILENAME)),
	   entry_filename->nameLength * 2);
    unicode2ascii(filename, entry_filename->nameLength);
    // ファイルパスに結合する
    for(i = filepath_len; i >= 0; i--) {
      filepath[i + entry_filename->nameLength + 1] = filepath[i];
    }
    for(i = 0; i < entry_filename->nameLength; i++) {
      filepath[i + 1] = filename[i];
    }
    filepath[0] = '\\';
    // 次に備える
    filepath_len += entry_filename->nameLength + 1;
    nextref = entry_filename->parentDir;
    // $MFTなら終了
    if (nextref == preref) break;
    preref = nextref;
    // 解放
    free(tmft, ntfs_info.sectorsPerRecord);
  }

  filepath[0] = 'C';
  filepath[1] = ':';

  // 不要な領域を解放
 getpath_return:
  free(tmft, ntfs_info.sectorsPerRecord);
  free(filename, 1);

  return filepath;
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
    for(j = 0; j < 0xFFFF; j++) {
      // 対象のレコードを取得する
      tmft = mft_list;
      
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
