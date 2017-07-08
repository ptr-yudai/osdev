/*
 * ide.c - ハードディスク関連
 */
#include "ide.h"
#include "pic.h"

#include "../include/io.h"

/*
 * IDEを初期化する
 */
void ide_init(void)
{
  if (ide_identify(ATA_PRIMARY, ATA_MASTER)) {
    fb_print("[DEBUG] Hard disk is recognized.\n");
    // TODO
  } else {
    fb_print("[DEBUG] Hard disk is not recognized.\n");
  }
  ide_identify(ATA_PRIMARY, ATA_SLAVE);
}

/*
 * ドライブを選択する
 *
 * @param bus
 * @param drive
 */
void ide_select_drive(u_char bus, u_char drive)
{
  if (bus == ATA_PRIMARY) {
    if (drive == ATA_MASTER) {
      outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
    } else {
      outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    }
  } else {
    if (drive == ATA_MASTER) {
      outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
    } else {
      outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    }
  }
}

/*
 * IDEを識別する
 *
 * @param bus 
 * @param drive 
 */
u_char ide_identify(u_char bus, u_char drive)
{
  int i;
  u_short io = 0;
  u_char status;
  // ドライブを選択
  ide_select_drive(bus, drive);
  if (bus == ATA_PRIMARY) {
    io = ATA_PRIMARY_IO;
  } else {
    io = ATA_SECONDARY_IO;
  }
  // 初期化
  outb(io + ATA_REG_SECCOUNT0, 0);
  outb(io + ATA_REG_LBA0, 0);
  outb(io + ATA_REG_LBA1, 0);
  outb(io + ATA_REG_LBA2, 0);
  // 識別要求
  outb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
  status = inb(io + ATA_REG_STATUS);
  if (status) {
    // ビジー待機
    while((inb(io + ATA_REG_STATUS) & ATA_SR_BSY) != 0);
  pm_stat_read:
    status = inb(io + ATA_REG_STATUS);
    // エラー
    if(status & ATA_SR_ERR) {
      return 0;
    }
    while(!(status & ATA_SR_DRQ)) goto pm_stat_read;
    for(i = 0; i < 256; i++) {
      char hoge[4] = {0};
      *(u_short*)hoge = inw(io + ATA_REG_DATA);
      fb_print(hoge);
    }
    return 1;
  }
  return 0;
}
