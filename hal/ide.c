/*
 * ide.c - ハードディスク関連
 */
#include "ide.h"
#include "pic.h"
#include "idt.h"
#include "irq.h"

#include "../include/io.h"

/*
 * IDEを初期化する
 */
void ide_init(void)
{
  int i;

  // ATA割り込みハンドラ
  idt_setup_ir(46, irq_ide);
  idt_setup_ir(47, irq_ide);

  if (ide_identify(ATA_PRIMARY, ATA_MASTER)) {
    fb_print("[DEBUG] Hard disk is recognized.\n");
    for(i = 0; i < 38; i += 2) {
      hdd_info.name[i] = ide_buffer[ATA_IDENT_MODEL + i + 1];
      hdd_info.name[i + 1] = ide_buffer[ATA_IDENT_MODEL + i];
    }
    fb_print("[DEBUG] New device: ");
    fb_print(hdd_info.name);
    fb_print("\n");
  } else {
    fb_print("[DEBUG] Hard disk is not recognized.\n");
  }
  ide_identify(ATA_PRIMARY, ATA_SLAVE);
}

/*
 * 指定セクタ分読み込む
 *
 * @param buf 読み込み先
 * @param lba LBA
 * @param n   読み込みセクタ数
 */
u_char ata_read(char *buf, u_char lba, u_char n)
{
  int i;
  u_short io = ATA_PRIMARY_IO;
  u_char drive = ATA_MASTER;
  u_char cmd = drive == ATA_MASTER ? 0xE0 : 0xF0;
  //u_char slavebit = drive == ATA_MASTER ? 0x00 : 0x01;

  for(i = 0; i < n; i++) {
    outb(io + ATA_REG_HDDEVSEL, (cmd | (u_char)((lba >> 24 & 0x0F))));
    outb(io + 1, 0x00);
    outb(io + ATA_REG_SECCOUNT0, 1);
    outb(io + ATA_REG_LBA0, (u_char)((lba)));
    outb(io + ATA_REG_LBA1, (u_char)((lba) >> 8));
    outb(io + ATA_REG_LBA2, (u_char)((lba) >> 16));
    outb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    ide_poll(io);
  
    for(i = 0; i < 256; i++) {
      *(u_short*)(buf + i * 2) = inw(io + ATA_REG_DATA);
    }
    lba++;
  }


  return 1;
}

/*
 * リソース待ち
 *
 * @param io IOポート
 */
void ide_poll(u_short io)
{
  int i;
  u_char status;
  for(i = 0; i < 4; i++) {
    inb(io + ATA_REG_ALTSTATUS);
  }
  // BSYがクリアされるのを待つ
  while(inb(io + ATA_REG_STATUS) & ATA_SR_BSY);
 retry:
  status = inb(io + ATA_REG_STATUS);
  if(status & ATA_SR_ERR) {
    fb_print("[WARNING] Something is wrong with HDD!\n");
    return;
  }
  if(!(status & ATA_SR_DRQ)) goto retry;
  return;
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
    for(i = 0; i < 128; i++) {
      *(u_short*)(ide_buffer + i*2) = inw(io + ATA_REG_DATA);
    }
    return 1;
  }
  return 0;
}
