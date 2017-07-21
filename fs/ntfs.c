#include "ntfs.h"

/*
 * NTFSのブートセクタをロードする
 *
 * @param mbr MBR構造体のポインタ
 * @return NTFS_BS構造体のポインタ
 */
NTFS_BS* ntfs_bootsector(MBR* mbr)
{
  NTFS_BS* bootsector = (NTFS_BS*)malloc(1);
  // [TODO] pTable1以外も参照
  ata_read((char*)bootsector, mbr->pTable1.lbaFirst, 1);
  // 必要な情報は保存
  ntfs_info.lbaFirst[0] = mbr->pTable1.lbaFirst;
  ntfs_info.bytesPerSector = bootsector->bpb.bytesPerSector;
  ntfs_info.sectorsPerCluster = bootsector->bpb.sectorsPerCluster;
  // 謎仕様によりMFTのサイズを計算
  if (bootsector->clustersPerMFT >= 0) {
    // clusterPerMFT >= 0ならセクタ/クラスタをかける
    ntfs_info.sectorsPerRecord = bootsector->clustersPerMFT * bootsector->bpb.sectorsPerCluster;
    ntfs_info.bytesPerRecord = ntfs_info.sectorsPerRecord * ntfs_info.bytesPerSector;
  } else {
    // clusterPerMFT < 0なら 2^(-clusterPerMFT) がMFTのバイト数
    ntfs_info.sectorsPerRecord = 1 << -bootsector->clustersPerMFT;
    ntfs_info.bytesPerRecord = ntfs_info.sectorsPerRecord;
    if (ntfs_info.sectorsPerRecord % ntfs_info.bytesPerSector == 0) {
      ntfs_info.sectorsPerRecord /= ntfs_info.bytesPerSector;
    } else {
      ntfs_info.sectorsPerRecord /= ntfs_info.bytesPerSector;
      ntfs_info.sectorsPerRecord++;
    }
  }
  return bootsector;
}

/*
 * MFTをロードする
 *
 * @param mftCluster MFT開始位置のクラスタ数
 * @return NTFS_MFT構造体のポインタ
 */
NTFS_MFT* ntfs_mft(u_int mftCluster)
{
  // [TODO] メモリ削減のためsectorsPerRecordではなく[secPerRec / 4KB]を使うこと
  NTFS_MFT* mft = (NTFS_MFT*)malloc(ntfs_info.sectorsPerRecord);
  // MFTを読み込む
  u_int64 access = (u_int64)mftCluster * ntfs_info.sectorsPerCluster;
  ata_read_ntfs((char*)mft, access, ntfs_info.sectorsPerRecord);
  // MFTの本体を探索する
  // [DEBUG] 探索する属性は引数で受け取るように
  NTFS_ATTR_HEADER_NR *mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA);
  if (mft_data == NULL) {
    fb_print("[DEBUG] Cannot find the attribute!");
    exit();
  }
  
  return mft;
}

/*
 * 属性と一致する領域を探索する
 *
 * @param mftHeader MFT領域の先頭へのポインタ
 * @param typeID    探索する属性
 */
void* ntfs_find_attribute(NTFS_MFT* mftHeader, u_short typeID)
{
  void* ptr = (void*)((u_int)mftHeader + mftHeader->attribOffset);
  u_int mftSize = ntfs_info.bytesPerRecord;

  while(1) {
    // 範囲外
    if (ptr + sizeof(NTFS_ATTR_HEADER_R) > (void*)((u_int)mftHeader + mftSize)) {
      break;
    }
    // 終端
    NTFS_ATTR_HEADER_R *header = (NTFS_ATTR_HEADER_R*)ptr;
    if (header->typeID == (u_int)0xFFFFFFFF) {
      break;
    }
    // 破損データによる無限ループを避ける
    if (header->length == 0) {
      break;
    }

    // 属性を発見
    if (header->typeID == typeID
	&& ptr + header->length <= (void*)((u_int)mftHeader + mftSize)) {
      return ptr;
    }
    
    ptr += header->length;
  }

  return NULL;
}

/*
 * NTFSの先頭からリードする
 */
void ata_read_ntfs(char *buf, u_char lba, u_char n)
{
  ata_read(buf, ntfs_info.lbaFirst[0] + lba, n);
}
