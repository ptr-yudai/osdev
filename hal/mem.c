/*
 * mem.c - 物理メモリ管理
 */
#include "mem.h"
#include "../include/util.h"

/*
 * メモリ管理を初期化する
 *
 * @param memsize メモリサイズ(バイト)
 */
void mem_init(u_int memsize)
{
  pm_info.mem_size     = memsize;
  pm_info.blocks_max   = memsize / MEMORY_BLOCK_SIZE;
  pm_info.blocks_used  = pm_info.blocks_max;
  pm_info.blocks_free  = 0;
  pm_info.mem_map      = (u_int*)(MEMORY_KERNEL_START + sizeof_kernel());
  pm_info.mem_map_size = pm_info.blocks_max / 32;
  memset(pm_info.mem_map, 0xFF, pm_info.mem_map_size);
}

/*
 * カーネルのサイズを取得する
 *
 * @return カーネルのサイズ(byte)
 */
inline u_int sizeof_kernel(void)
{
  return 0;
  //return (_BSS_END - _TEXT_START);
}
