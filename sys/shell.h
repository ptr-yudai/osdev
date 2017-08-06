#ifndef SYS_SHELL_H
#define SYS_SHELL_H

#include "../include/types.h"
#include "../include/io.h"
#include "../fs/ntfs_scanner.h"

/*----- 構造体定義 -----*/
typedef struct {
  u_int mftref;    // カレントディレクトリ
  u_int mftSector; // $MFTの開始セクタ
} SHELL_INFO;

/*----- 関数定義 -----*/
void k_shell(void);


/*----- 変数定義 -----*/
SHELL_INFO sh_info;

#endif

