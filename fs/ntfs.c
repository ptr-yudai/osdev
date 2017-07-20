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
  //ntfs_info.bytesPerCluster = ntfs_info.bytesPerSector * ntfs_info.sectorsPerCluster;
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
  NTFS_MFT* mft = (NTFS_MFT*)malloc(1);
  // MFTを読み込む
  u_int64 access = (u_int64)mftCluster * ntfs_info.sectorsPerCluster;
  ata_read_ntfs((char*)mft, access, 1);
  fb_print((char*)mft->signature);
  return mft;
}

/*
 * NTFSの先頭からリードする
 */
void ata_read_ntfs(char *buf, u_char lba, u_char n)
{
  ata_read(buf, ntfs_info.lbaFirst[0] + lba, n);
}
