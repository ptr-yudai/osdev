#ifndef FS_NTFSSCAN_H
#define FS_NTFSSCAN_H

#include "../include/types.h"
#include "../include/util.h"
#include "../include/time.h"
#include "../include/io.h"
#include "../sys/screen.h"
#include "ntfs.h"

/*----- 関数定義 -----*/
u_int ntfs_mmls(void);
void ntfs_fls(u_int mftSector);
void ntfs_icat(u_int mftSector, u_int64 mftref);
void ntfs_investigate(u_int mftCluster);

#endif
