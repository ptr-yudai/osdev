#ifndef FS_NTFS_H
#define FS_NTFS_H

#include "../include/types.h"
#include "../include/util.h"
#include "../include/io.h"
#include "../hal/ide.h"
#include "mbr.h"

/*----- 構造体 -----*/
// ブートセクタ
typedef struct {
  u_char jmpIPL[3];
  u_char OEM[8];
  u_char secSize[2];
  u_char secPerClus;
  u_char rsvSecCount[2];
  u_char fatCount;
  u_char rootDirCount[2];
  u_char secCount[2];
  u_char mediaDesc;
  u_char secPerFAT[2];
  u_char secPerTrck[2];
  u_char headCount[2];
  u_char hiddenSec[4];
  u_char bigSecCount[4];
  u_char reserved1[4];
  u_char ntfsSecCount[8];
  u_char mftStartClus[8];
  u_char mftMirrStartClus[8];
  u_char clusPerFRS[4];
  u_char clusPerIndex[4];
  u_char serialNum[4];
  u_char checksum[4];
} NTFS_BOOTSECTOR;

/*----- 関数定義 -----*/
NTFS_BOOTSECTOR* ntfs_bootsector(MBR* mbr);

#endif
