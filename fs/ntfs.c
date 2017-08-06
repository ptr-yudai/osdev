#include "ntfs.h"

/*
 * NTFSのブートセクタをロードする
 *
 * @param mbr MBR構造体のポインタ
 * @param num 参照するパーティションテーブル
 * @return NTFS_BS構造体のポインタ
 */
NTFS_BS* ntfs_bootsector(MBR* mbr, u_int num)
{
  NTFS_BS* bootsector = (NTFS_BS*)malloc(1);
  pTable *table;
  switch(num) {
  case 1: table = &mbr->pTable1; break;
  case 2: table = &mbr->pTable2; break;
  case 3: table = &mbr->pTable3; break;
  case 4: table = &mbr->pTable4; break;
  }
  ata_read((char*)bootsector, table->lbaFirst, 1);
  // 必要な情報は保存
  ntfs_info.lbaFirst[0] = table->lbaFirst;
  ntfs_info.bytesPerSector = bootsector->bpb.bytesPerSector;
  ntfs_info.sectorsPerCluster = bootsector->bpb.sectorsPerCluster;
  ntfs_info.numClusters = do_div64(bootsector->numSectors, bootsector->bpb.sectorsPerCluster);
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
NTFS_MFT* ntfs_mft(u_int mftSector)
{
  // [TODO] メモリ削減のためsectorsPerRecordではなく[secPerRec / 4KB]を使うこと
  NTFS_MFT* mft = (NTFS_MFT*)malloc(ntfs_info.sectorsPerRecord);
  // MFTを読み込む
  ata_read_ntfs((char*)mft, mftSector, ntfs_info.sectorsPerRecord);
  // MFTの本体を探索する
  // [DEBUG] 探索する属性は引数で受け取るように
  /*
  NTFS_ATTR_HEADER_NR *mft_data = (NTFS_ATTR_HEADER_NR*)ntfs_find_attribute(mft, NTFS_MFT_ATTRIBUTE_DATA);
  if (mft_data == NULL) {
    fb_print("[DEBUG] Cannot find the attribute!");
    exit();
  }
  */
  return mft;
}

/*
 * 属性と一致する領域を探索する
 *
 * @param mftHeader MFT領域の先頭へのポインタ
 * @param typeID    探索する属性
 */
void *ntfs_find_attribute(NTFS_MFT* mftHeader, u_short typeID)
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
    // 次のエントリへ
    ptr += header->length;
  }
  return NULL;
}

/*
 * runlistを読み込む
 *
 * @param entry  NTFS_ATTR_HEADER_NR構造体へのポインタ
 * @param runlist NTFS_RUNLIST構造体へのポインタ
 */
NTFS_RUNLIST* ntfs_parse_runlist(NTFS_ATTR_HEADER_NR *entry)
{
  int i;
  void* runList = (char*)entry + entry->runListOffset;
  u_int runListSize = entry->length - entry->runListOffset;
  void* ptr = runList;

  char failFlag = 0;
  NTFS_RUNLIST *datarun = (NTFS_RUNLIST*)malloc(1);
  NTFS_RUNLIST *datarun_ret = datarun;
  char SIZE_RUNLIST = sizeof(NTFS_RUNLIST);
  
  long long int cp = 0LL;
  
  while((char)*(char*)ptr) {
    // 範囲外
    if (ptr + 1 > runList + runListSize) {
      failFlag = 1;
      break;
    }
    // runlistを解読する
    char lenLength = (char)*(char*)ptr & 0x0f; // 長さのビット数
    char lenOffset = (char)*(char*)ptr >> 4;   // オフセットのビット数
    ptr++;
    // 終端
    if (lenOffset == 0) {
      failFlag = 1;
      break;
    }
    // おかしなデータ
    if (ptr + lenLength + lenOffset > runList + runListSize
	|| lenLength >= 8
	|| lenOffset >= 8) {
      failFlag = 1;
      break;
    }
    // サイズとオフセットを取得
    u_int64 length = 0;
    long long int offset = 0;
    for(i = 0; i < lenLength; i++, ptr++) {
      // lenLengthバイトのlength情報をコピー
      length |= (u_char)*(u_char*)ptr << (i * 8);
    }
    for(i = 0; i < lenOffset; i++, ptr++) {
      // lenOffsetバイトのoffset情報をコピー
      offset |= (u_char)*(u_char*)ptr << (i * 8);
    }
    // 負のオフセットも考える
    if (offset >= (1 << (lenOffset * 8 - 1))) {
      offset -= 1LL << (lenOffset * 8);
    }
    cp += offset;
    // 範囲外
    if (cp < 0 || ntfs_info.numClusters < cp) {
      failFlag = 1;
      break;
    }
    
    datarun->offset = cp;
    datarun->length = length;
    datarun = (NTFS_RUNLIST*)((char*)datarun + SIZE_RUNLIST);
  }
  if (failFlag) {
    // 失敗
    free(datarun, 1);
    return NULL;
  }

  return datarun_ret;
}

/*
 * Datarunのn番目のINDEXレコードを取得する
 *
 * @param runlist ntfs_parse_runlistで取得したDatarun
 * @param n       取得したいINDEXレコードの番号(Datarunの何番目か)
 */
void *ntfs_find_data(NTFS_RUNLIST *runlist, u_int n)
{
  u_int i;
  void *index;
  // Datarunをずらす
  for(i = 0; i < n; i++) {
    runlist = (NTFS_RUNLIST*)((char*)runlist + sizeof(NTFS_RUNLIST));
  }
  if (runlist->offset == 0 || runlist->length == 0) {
    return NULL;
  }
  // INDEXレコードを取得
  index = (void*)malloc(runlist->length);
  ata_read_ntfs((char*)index,
		runlist->offset * ntfs_info.sectorsPerCluster,
		runlist->length * ntfs_info.sectorsPerCluster);
  fb_printf("0x%x\n", runlist->offset * ntfs_info.sectorsPerCluster);
  return index;
}

/*
 * NTFSの先頭からリードする
 */
void ata_read_ntfs(char *buf, u_int lba, u_int n)
{
  ata_read(buf, ntfs_info.lbaFirst[0] + lba, n);
}
