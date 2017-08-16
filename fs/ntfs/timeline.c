#include "ntfs_scanner.h"

/**
 * timeline - タイムラインを作成する
 *
 * @param mftSector $MFTの開始セクタ番号
 */
void ntfs_timeline(u_int mftSector)
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
    goto timeline_return;
  }

  // DATA属性を取得(ほぼ間違いなくnon-resident)
  mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA, 0);
  // [TODO] residentの実装(優先度低)
  if (mft_data->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    fb_debug("Resident $MFT is not supported!\n", ER_FATAL);
    goto timeline_return;
  }
  // datarunを取得
  runlist = ntfs_parse_runlist(mft_data);
  if (runlist == NULL) {
    scr_switch(0);
    fb_debug("Invalid datarun!\n", ER_WARNING);
    goto timeline_return;
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
      tmft = tmft;
      
      // IDをチェック
      // [TODO] ここでチェック
      //
      //
      //
      //
      //
      
      mft_list += ntfs_info.bytesPerSector * ntfs_info.sectorsPerRecord;
    }

    free(mft_list, trun->length);
  }

 timeline_return:
  free(runlist, 1);
  free(mft, ntfs_info.sectorsPerRecord);
}
