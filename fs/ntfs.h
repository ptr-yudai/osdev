#ifndef FS_NTFS_H
#define FS_NTFS_H

#include "../include/types.h"
#include "../include/util.h"
#include "../include/io.h"
#include "../hal/ide.h"
#include "mbr.h"

/*----- 構造体 -----*/
// BIOS Parameter Block
typedef struct {
  u_short bytesPerSector; 
  u_char  sectorsPerCluster;
  u_short reservedSectors;
  u_char  fats;
  u_short rootEntries;
  u_short sectors;
  u_char  mediaType;
  u_short sectorsPerFAT;
  u_short sectorsPerTrack;
  u_short heads;
  u_int   hiddenSectors;
  u_int   bigSectors;
} __attribute__((__packed__)) NTFS_BPB;

// ブートセクタ
typedef struct {
  u_char   jmp[3];           // ジャンプコード
  u_char   oem[8];           // OEM ID
  NTFS_BPB bpb;              // BIOS Parameter Block
  u_char   reserved1[4];     // 未使用
  s_int64  numSectors;       // セクタ数
  s_int64  mftCluster;       // MFTの開始クラスタ
  s_int64  mftmirrCluster;   // MFT Mirrorの開始クラスタ
  s_char   clustersPerMFT;   // MFTあたりのクラスタ数
  u_char   reserved2[3];     // 予約済み
  s_char   clustersPerIndex; // インデクスブロックあたりのクラスタ数
  u_char   reserved3[3];     // 予約済み
  u_int64  serialNum;        // シリアル番号
  u_int    checksum;         // チェックサム
  u_char   bootstrap[426];   // ブートコード
  u_short  signature;        // シグネチャ
} __attribute__((__packed__, __aligned__(8))) NTFS_BS;

// マスターファイルテーブル
typedef struct {
  u_char  signature[4];
  u_short fixupOffset;
  u_short fixupSize;
  u_int64 logSeqNumber;
  u_short sequence;
  u_short hardlinks;
  u_short attribOffset;
  u_short flags;
  u_int   recLength;
  u_int   allLength;
  u_int64 baseMFTRec;
  u_int   nextAttrID;
  u_int   fixupPattern;
  u_short MFTRecNumber;
} __attribute__((__packed__)) NTFS_MFT;

// 共用情報
typedef struct {
  u_short lbaFirst[4];
  u_short bytesPerSector;
  u_char  sectorsPerCluster;
  //u_int   bytesPerCluster;
} NTFS_INFO;

/*----- 関数定義 -----*/
NTFS_BS* ntfs_bootsector(MBR* mbr);
NTFS_MFT* ntfs_mft(u_int mftCluster);
void ata_read_ntfs(char *buf, u_char lba, u_char n);

/*----- 変数定義 -----*/
NTFS_INFO ntfs_info;

#endif
