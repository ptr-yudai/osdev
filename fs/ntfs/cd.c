#include "ntfs_scanner.h"

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
