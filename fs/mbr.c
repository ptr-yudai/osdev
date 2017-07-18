#include "mbr.h"
#include "../hal/ide.h"
#include "../include/util.h"
#include "../include/io.h"

/*
 * MBR領域を読み込む
 *
 * @return MBR情報へのポインタ
 */
MBR* mbr_load(void)
{
  MBR* mbr = (MBR*)malloc(1);
  // MBRのロード
  ata_read((char*)mbr, 0, 1);
  // シグネチャの確認
  if (mbr->signature[0] != (u_char)'\x55'
      || mbr->signature[1] != (u_char)'\xAA') {
    fb_print("[STOP] Unsupported MBR version.\n");
    // [TODO]
    // MBR修復用にパーティションを強制的に探索するコードを入れる
    exit();
  }
  fb_print("[DEBUG] MBR is loaded correctly.\n");
  return mbr;
}

