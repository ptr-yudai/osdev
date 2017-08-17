#include "ntfs_scanner.h"

/**
 * icat - 特定のファイル内容を取得する
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Reference Number (ID)
 * @param adsid     何番目のADSを取得するか
 * @param mode      どのように出力するか
 * @param param     modeに対応した入力
 */
void ntfs_icat(u_int mftSector, u_int64 mftref, u_short adsid, u_char mode, u_int param)
{
  u_int i;
  char* filename = (char*)malloc(1);     // ファイル名
  NTFS_MFT              *tmft;           // MFTレコード
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
  // 不正なMFT
  if (!((tmft->signature[0] == 'F') &
	(tmft->signature[1] == 'I') &
	(tmft->signature[2] == 'L') &
	(tmft->signature[3] == 'E'))) {
    tmft = NULL;
    goto icat_return;
  }

  // DATA属性を取得
  mft_data = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_DATA, adsid);
  if (mft_data == NULL) {
    fb_debug("No DATA attribute found.\n", ER_CATION);
    goto icat_return;
  }
  // Resident
  if (mft_data->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    data = ((char*)mft_data + mft_data->contentOffset);
    switch(mode) {
    case NTFS_ICAT_MODE_HEX: // ヘックスダンプ
      fb_hexdump(data, param);
      break;

    default: // その他
      fb_printf("%s\n", data);
      break;
    }
  } else {
    runlist = ntfs_parse_runlist((NTFS_ATTR_HEADER_NR*)mft_data);
    // [TODO] 全部取得すること
    NTFS_RUNLIST *trun = NULL;
    for(i = 0; ; i++) {
      trun = ntfs_extract_runlist(runlist, i);
      data = (char*)ntfs_find_data(runlist, i);
      switch(mode) {
      case NTFS_ICAT_MODE_HEX: // ヘックスダンプ
	fb_hexdump(data, param);
	break;
      default: // その他
	fb_printf("%s\n", data);
	break;
      }
      free(data, trun->length);
    }
    free(data, trun->length);
    free(runlist, 1);
  }
  
  // 不要な領域を解放
 icat_return:
  free(tmft, ntfs_info.sectorsPerRecord);
  free(filename, 1);
}
