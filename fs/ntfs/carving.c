#include "ntfs_scanner.h"

/**
 * carving - ファイルカービング(もしくは探索)を実行する
 *
 * @param data   データ本体へのポインタ
 * @param size   探索サイズ（バイト）
 * @param option 何をベースに探索するか
 * @param b_clue optionに対するバイナリデータ
 * @param i_clue optionに対する数値データ
 */
void ntfs_carving(char* data, u_int size, u_int option, char* b_clue, int i_clue)
{
  int i;
  char *ptr, *end_ptr;
  char* filename = (char*)malloc(1);     // ファイル名
  NTFS_MFT              *mft;            // MFTレコード
  NTFS_ATTR_HEADER_R    *attr_header;    // 属性のヘッダ
  NTFS_ENTRY_FILENAME   *entry_filename; // FILENAMEエントリ

  switch(option) {
  case NTFS_CARVING_FILENAME: // ファイル名で検索
    end_ptr = data + size;
    for(ptr = data; ptr < end_ptr; ++ptr) {
      if (strncmp(ptr, "FILE", 4) && strncmp(ptr, "BAAD", 4)) continue;
      // MFTシグネチャを発見
      mft = (NTFS_MFT*)ptr;
      // ファイル名を調べる
      for(i = 0; ; i++) {
	attr_header = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_FILENAME, i);
	if (attr_header == NULL) {
	  // 1つも見つからない場合は次へ
	  if (i == 0) break;
	  // 見つからない場合は前のものを使う
	  attr_header = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_FILENAME, i - 1);
	  break;
	}
      }
      if (attr_header == NULL) continue;

      entry_filename = (NTFS_ENTRY_FILENAME*)((char*)attr_header + attr_header->contentOffset);
      memcpy(filename,
	   (void*)((char*)entry_filename + sizeof(NTFS_ENTRY_FILENAME)),
	   entry_filename->nameLength * 2);
      unicode2ascii(filename, entry_filename->nameLength);
      // ファイル名を比較
      if (strncmp(filename, b_clue, strlen(b_clue)) == 0) {
	fb_printf("Found %s (MFT Reference Number = 0x%x)\n", b_clue, mft->MFTRecNumber);
	break;
      }
    }
    break;

  case NTFS_CARVING_FILESIZE: // ファイルサイズで検索
    fb_printf("%d\n", i_clue);
    break;

  default: // 不明なオプション
    fb_debug("Unsupported option.\n", ER_CATION);
    break;
  }

  free(filename, 1);
}
