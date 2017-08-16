#include "ntfs_scanner.h"

/**
 * istat - 特定のファイルの詳細情報を取得する
 *
 * @param mftSector $MFTの開始セクタ番号
 * @param mftref    MFT Referenec Number (ID)
 */
void ntfs_istat(u_int mftSector, u_int mftref)
{
  char* filename = (char*)malloc(1);     // ファイル名
  NTFS_MFT              *tmft;           // MFTレコード
  NTFS_ATTR_HEADER_R    *mft_header;     // 属性のヘッダ
  NTFS_ENTRY_FILENAME   *entry_filename; // FILENAMEエントリ
  NTFS_ENTRY_STDINFO    *entry_stdinfo;  // STANDARD_INFORMATIONエントリ
  u_int64               unixtime;        // UNIXタイム
  DATETIME              datetime;        // DATETIME構造体

  // 参照番号に一致したMFTを取得
  tmft = ntfs_getrecord(mftSector, mftref);
  if (tmft == NULL) {
    scr_switch(1);
    fb_debug("Specified file or directory not found.\n", ER_CATION);
    goto istat_return;
  }
  // 不正なMFT
  if (!((tmft->signature[0] == 'F') &
	(tmft->signature[1] == 'I') &
	(tmft->signature[2] == 'L') &
	(tmft->signature[3] == 'E'))) {
    tmft = NULL;
    goto istat_return;
  }
  
  // FILENAME属性を取得
  mft_header = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_FILENAME, 0);
  // Resident
  if (mft_header->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    
    // FILENAME属性を取得
    entry_filename = (NTFS_ENTRY_FILENAME*)((char*)mft_header + mft_header->contentOffset);
    memcpy(filename,
	   (void*)((char*)entry_filename + sizeof(NTFS_ENTRY_FILENAME)),
	   entry_filename->nameLength * 2);
    unicode2ascii(filename, entry_filename->nameLength);
    fb_printf("Filename: %s\n", filename);
  } else {
    fb_debug("Non-resident FILENAME is not supported!\n", ER_FATAL);
  }
  
  // STANDARD_INFORMATION属性を取得
  mft_header = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(tmft, NTFS_MFT_ATTRIBUTE_STDINFO, 0);
  // Resident
  if (mft_header->formCode == NTFS_MFT_ATTRIBUTE_RESIDENT) {
    entry_stdinfo = (NTFS_ENTRY_STDINFO*)((char*)mft_header + mft_header->contentOffset);
    // 作成日時
    unixtime = ts_file2unix(entry_stdinfo->tsCreated);
    ts_unix2date(unixtime, &datetime);
    fb_printf("Created : %d-%d-%d %d:%d:%d\n",
	      datetime.year, datetime.month, datetime.day,
	      datetime.hour, datetime.minute, datetime.second);
    // 更新日時
    unixtime = ts_file2unix(entry_stdinfo->tsModified);
    ts_unix2date(unixtime, &datetime);
    fb_printf("Modified: %d-%d-%d %d:%d:%d\n",
	      datetime.year, datetime.month, datetime.day,
	      datetime.hour, datetime.minute, datetime.second);
    // アクセス日時
    unixtime = ts_file2unix(entry_stdinfo->tsAccessed);
    ts_unix2date(unixtime, &datetime);
    fb_printf("Accessed: %d-%d-%d %d:%d:%d\n",
	      datetime.year, datetime.month, datetime.day,
	      datetime.hour, datetime.minute, datetime.second);
  } else {
    fb_debug("Non-resident STANDARD_INFORMATION is not supported!\n", ER_FATAL);
  }
  
  // 不要な領域を解放
 istat_return:
  free(tmft, ntfs_info.sectorsPerRecord);
  free(filename, 1);
}
