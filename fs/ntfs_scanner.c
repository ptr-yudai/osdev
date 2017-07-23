#include "ntfs_scanner.h"

/*
 * NTFSの調査開始
 */
void ntfs_investigate(u_int mftSector)
{ 
  int i, n;
  char *filename = (char*)malloc(1);
  NTFS_MFT *file_mft;
  NTFS_ATTR_HEADER_R *mft_filename;
  NTFS_ENTRY_FILENAME *attr_filename;
  
  for(i = 0; i < 12; i++) {
    // MFTエントリを読み込み
    file_mft = ntfs_mft(mftSector + ntfs_info.sectorsPerRecord * i);
    switch(file_mft->flags) {
    case 0x00: fb_print("[DEL    ] "); break;
    case 0x01: fb_print("[       ] "); break;
    case 0x02: fb_print("[DEL DIR] "); break;
    case 0x03: fb_print("[    DIR] "); break;
    default:   fb_print("[BROKEN!] "); break;
    }
    // FILENAME属性を探索
    mft_filename = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(file_mft, NTFS_MFT_ATTRIBUTE_FILENAME);
    // FILENAME属性本体を読み込み
    attr_filename = (NTFS_ENTRY_FILENAME*)((char*)mft_filename + mft_filename->contentOffset);
    // ファイル名を取得
    memcpy(filename,
	   (void*)((char*)attr_filename + sizeof(NTFS_ENTRY_FILENAME)),
	   attr_filename->nameLength * 2);
    unicode2ascii(filename, attr_filename->nameLength);
    fb_printf("%d --> %s\n", i, filename);
    // 解放
    free(file_mft, 1);
    free(mft_filename, 1);
    free(attr_filename, 1);
  }

  n = -1;
  while(n < 0 || n > 12) {
    fb_print("Enter the entry number you want to investigate: ");
    if (kb_getnumber(&n) == 0) {
      fb_print("[ERROR] Invalid number!\n");
      n = -1;
      continue;
    }
  }
  fb_printf("%d is selected.\n", n);
  
  // 解放
  free(filename, 1);
}
