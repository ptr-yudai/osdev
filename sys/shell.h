#ifndef SYS_SHELL_H
#define SYS_SHELL_H

#include "../include/types.h"
#include "../include/io.h"
#include "../fs/ntfs/ntfs_scanner.h"

/*----- 定数定義 -----*/
#define FREE_CHECK_FUEL   64
#define SHELL_CMD_LENGTH  64
#define SHELL_ARGV_SIZE   8
#define SHELL_ARGV_LENGTH 32

/*----- 構造体定義 -----*/
typedef struct {
  u_int mftref;    // カレントディレクトリ
  u_int mftSector; // $MFTの開始セクタ
} SHELL_INFO;

/*----- 関数定義 -----*/
void k_shell(void);
void sh_setvar(char* vname, char* data);

/*----- 変数定義 -----*/
SHELL_INFO sh_info;

#endif

