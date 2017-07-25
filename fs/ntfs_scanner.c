#include "ntfs_scanner.h"

/*
 * NTFSの調査開始
 */
void ntfs_investigate(u_int mftSector)
// 今は決め打ちでルートディレクトリを選択
{
  char *filename = (char*)malloc(1);
  NTFS_MFT *mft;
  NTFS_ATTR_HEADER_R *mft_iroot;
  NTFS_ATTR_HEADER_NR *mft_iallc;
  NTFS_ENTRY_INDXROOT *attr_iroot;
  NTFS_INODE_I30_HEADER *inode;
  NTFS_RUNLIST *runlist = NULL;
  NTFS_RECORD_INDEX *rec_index;
  // FILEレコードを取得
  mft = ntfs_mft(mftSector + ntfs_info.sectorsPerRecord * 5);
  /*----- INDEX_ROOTを確認 -----*/
  // INDEX_ROOTを取得
  mft_iroot = (NTFS_ATTR_HEADER_R*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXROOT);
  // ストリーム名を取得
  memcpy((void*)filename,
	 (void*)((char*)mft_iroot + mft_iroot->nameOffset),
	 mft_iroot->nameLength * 2);
  unicode2ascii(filename, mft_iroot->nameLength);
  fb_printf("Stream name is \"%s\"\n", filename);
  attr_iroot = (NTFS_ENTRY_INDXROOT*)((char*)mft_iroot + mft_iroot->contentOffset);
  // INODE_I30を確認
  inode = (NTFS_INODE_I30_HEADER*)((char*)attr_iroot + 0x10 + attr_iroot->inode.inodeOffset);
  if (inode->flags != NTFS_MFT_ENTRY_FLAGS_SMALLINDX) {
    /*----- INDEX_ALLOCATIONを確認 -----*/
    // リストがINDEX_ALLOCに存在する(必ずNon-resident)
    mft_iallc = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_INDXALLC);
    // datarunを取得
    runlist = ntfs_parse_runlist(mft_iallc);
    if (runlist == NULL) {
      fb_print("[WARNING] Invalid datarun!\n");
    } else {
      fb_print("[INFO] It works!\n");
    }
    rec_index = ntfs_find_index(runlist, 0);
    fb_printf("%s\n", rec_index->signature);

    /*----- INDEX_RECORDのファイル一覧を取得 -----*/
    inode = (NTFS_INODE_I30_HEADER*)((char*)rec_index + 0x18 + rec_index->inode.inodeOffset);
    int i;
    for(i = 0; i < 10; i++) {
      fb_printf("0x%x:", inode);
      fb_printf("0x%x ", inode->length);
      inode = (NTFS_INODE_I30_HEADER*)((char*)inode + inode->length);
    }

    free(rec_index, runlist->length);// [TODO] 正しくfree
  } else {
    // [TODO]実装
    fb_print("[DEBUG] INDEX is resident! Implement it!\n");
  }
  
  // free
  free(mft, ntfs_info.sectorsPerRecord);
  free(mft_iroot, 1);
  free(mft_iallc, 1);
  free(runlist, 1);
}
/*
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
    free(file_mft, ntfs_info.sectorsPerRecord);
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

  file_mft = ntfs_mft(mftSector + ntfs_info.sectorsPerRecord * n);
  if (file_mft->flags & NTFS_FILE_FLAG_DIRECTORY) {
    fb_print("It is a directory.\n");
  } else {
    fb_print("It is a file.\n");
  }
  free(file_mft, ntfs_info.sectorsPerRecord);
  
  // 解放
  free(filename, 1);
}
*/
