#ifndef FS_MBR_H
#define FS_MBR_H

#include "../include/types.h"

/*----- 定数定義 -----*/
// ファイルシステムの種別(MBR.pTable.type)
#define MBR_PTYPE_EMPTY  0x00
#define MBR_PTYPE_FAT12  0x01
#define MBR_PTYPE_FAT16  0x04
#define MBR_PTYPE_FAT16L 0x06
#define MBR_PTYPE_NTFS   0x07
#define MBR_PTYPE_FAT32  0x0B
#define MBR_PTYPE_FAT32X 0x0C
#define MBR_PTYPE_UNIX   0x63
#define MBR_PTYPE_LSWAP  0x82
#define MBR_PTYPE_EXT2   0x83
#define MBR_PTYPE_LINUX  0x85
#define MBR_PTYPE_NTFSR2 0x86
#define MBR_PTYPE_NTFSR1 0x87
#define MBR_PTYPE_FBSD   0xA5
#define MBR_PTYPE_OBSF   0xA6
#define MBR_PTYPE_NEXT   0xA7
// アクティブフラグ(pTable.bootflag)
#define PTABLE_ACTIVE    0x80
#define PTABLE_INACTIVE  0x00

/*----- 構造体 -----*/
// パーティションテーブル
typedef struct {
  u_char bootflag;
  u_char chsFirst[3];
  u_char type;
  u_char chsLast[3];
  u_char lbaFirst[4];
  u_char lbaCount[4];
} pTable;
// MBR
typedef struct {
  u_char mbr[446];     // Master Bootstrap Loader
  // パーティションテーブル
  pTable pTable1;
  pTable pTable2;
  pTable pTable3;
  pTable pTable4;
  u_short signature; // シグネチャ
} MBR;

/*----- 関数定義 -----*/
MBR* mbr_load(void);

#endif
