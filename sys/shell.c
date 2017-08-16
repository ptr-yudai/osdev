#include "shell.h"

/*
 * コマンド受付
 */
void k_shell(void)
{
  char cmd[SHELL_CMD_LENGTH];
  char argv[SHELL_ARGV_SIZE][SHELL_ARGV_LENGTH];
  char *ptr;
  int i, argc;
  void *freechk_b = NULL, *freechk_f = NULL;
  
  // 初期化
  sh_info.mftref = 0;
  sh_info.mftSector = 0;

  // コマンド受付
  while(1) {
    // free忘れを検知
    freechk_b = malloc(1);
    if (freechk_b > freechk_f) {
      fb_debug("Did you call free appropriately?\n", ER_CATION);
    }

    // コマンド入力
    scr_switch(1);
    fb_printf("[0x%x]# ", sh_info.mftref);
    kb_getline(cmd);

    freechk_f = freechk_b;
    free(freechk_b, 1);

    // 結果用画面を初期化
    scr_switch(2);
    fb_clrscr();

    // コマンドをパース
    memset(argv[0], '\x00', SHELL_ARGV_LENGTH);
    for(argc = 1, i = 0, ptr = cmd; *ptr != 0x00; i++, ptr++) {
      // 空白区切り
      if (*ptr == ' ') {
	// 空白が連続していない
	if (*(ptr - 1) != ' ') {
	  memset(argv[argc], '\x00', SHELL_ARGV_LENGTH);
	  argc++;
	}
	i = -1;
	continue;
      }
      argv[argc - 1][i] = *ptr;
    }
    // 最後がスペースだと多くカウントしてしまうので修正
    if (*(ptr - 1) == ' ') argc--;

    // mmls - パーティションの指定
    if (strncmp(argv[0], "mmls", 5) == 0) {
      sh_info.mftSector = ntfs_mmls();
      sh_info.mftref = NTFS_MFT_INODE_ROOTDIR;
    }
    // fls - ファイル一覧
    if (strncmp(argv[0], "fls", 4) == 0) {
      if (sh_info.mftSector == 0) {
	fb_debug("$MFT Sector is required. (not initialized)\n", ER_CATION);
      } else {
	if (argc == 1) {
	  // カレントディレクトリを参照
	  ntfs_ls(sh_info.mftSector, sh_info.mftref);
	} else if (argc == 2) {
	  // 引数が指定されればそのディレクトリを見る
	  ntfs_ls(sh_info.mftSector, atoi(argv[1], 16));
	} else if (argc == 3) {
	  if (strncmp(argv[1], "-f", 3) == 0) {
	    ntfs_fls(sh_info.mftSector, atoi(argv[2], 16));
	  }
	}
      }
    }
    // icat - ファイル内容取得
    if (strncmp(argv[0], "icat", 4) == 0) {
      if (sh_info.mftSector == 0) {
	fb_debug("$MFT Sector is required. (not initialized)\n", ER_CATION);
      } else {
	ntfs_icat(sh_info.mftSector, atoi(argv[1], 16));
      }
    }
    // istat - ファイル詳細取得
    if (strncmp(argv[0], "istat", 5) == 0) {
      if (sh_info.mftSector == 0) {
	fb_debug("$MFT Sector is required. (not initialized)\n", ER_CATION);
      } else {
	ntfs_istat(sh_info.mftSector, atoi(argv[1], 16));
      }
    }
    // cd - ディレクトリを移動
    if (strncmp(argv[0], "cd", 3) == 0) {
      if (sh_info.mftSector == 0) {
	fb_debug("$MFT Sector is required. (not initialized)\n", ER_CATION);
      } else {
	u_int nextref;
	nextref = ntfs_cd(sh_info.mftSector, atoi(argv[1], 16));
	if (nextref != ERROR) {
	  sh_info.mftref = nextref;
	}
      }
    }
    // debug - 情報を表示
    if (strncmp(argv[0], "debug", 6) == 0) {
      fb_printf("ntfs_info.bytesPerSector = 0x%x\n", ntfs_info.bytesPerSector);
      fb_printf("ntfs_info.sectorsPerCluster = 0x%x\n", ntfs_info.sectorsPerCluster);
      fb_printf("sh_info.mftSector = 0x%x\n", sh_info.mftSector);
      fb_printf("sh_info.mftref    = 0x%x\n", sh_info.mftref);
    }
    // set - 変数を変更
    if (strncmp(argv[0], "set", 4) == 0) {
      if (argc == 3) {
	sh_setvar(argv[1], argv[2]);
      } else {
	fb_debug("Usage: set [variable] [data]\n", ER_INFO);
      }
    }
  }
}

/*
 * 変数をセット
 */
void sh_setvar(char* vname, char* data)
{
  if (strncmp(vname, "mftSector", 10) == 0) {
    sh_info.mftSector = atoi(data, 16);
    fb_printf("sh_info.mftSector <-- 0x%x\n", atoi(data, 16));
  }
}
