#include "shell.h"

/*
 * コマンド受付
 */
void k_shell(void)
{
  char cmd[64];
  
  while(1) {
    scr_switch(1);
    fb_print("[/]# ");
    kb_getline(cmd);

    if (strncmp(cmd, "mmls", 5) == 0) {
      scr_switch(2);
      ntfs_mmls();
    }
  }
}
