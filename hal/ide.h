#ifndef HAL_IDE_H
#define HAL_IDE_H

#include "../include/types.h"
#include "../include/io.h"

/*----- 定数定義 -----*/
// 状態のビットマスク
#define ATA_SR_BSY  0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF   0x20
#define ATA_SR_DSC  0x10
#define ATA_SR_DRQ  0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX  0x02
#define ATA_SR_ERR  0x01
// 命令
#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC

/*----- 関数定義 -----*/
void ide_init(void);

#endif
