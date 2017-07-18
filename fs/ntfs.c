#include "ntfs.h"

NTFS_BOOTSECTOR* ntfs_bootsector(MBR* mbr)
{
  NTFS_BOOTSECTOR* bootsector = (NTFS_BOOTSECTOR*)malloc(1);
  ata_read((char*)bootsector, (u_short)*(mbr->pTable1.lbaFirst), 1);
  // [TODO] fix cast
  fb_printx((u_int)*(bootsector->checksum));
  return bootsector;
}

