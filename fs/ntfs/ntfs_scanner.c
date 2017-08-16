#include "ntfs_scanner.h"

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
      /*
      // [TODO] datarunのlengthまでを調べるのが正攻法
      if (!((tmft->signature[0] == 'F') &
	    (tmft->signature[1] == 'I') &
	    (tmft->signature[2] == 'L') &
	    (tmft->signature[3] == 'E'))) {
	// 次のdatarunへ移行
	tmft = NULL;
	break;
      }
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
