/*
 * mem.c - 物理メモリ管理
 */
#include "mem.h"
#include "../include/util.h"
#include "../include/linker.h"

#include "../include/io.h"

/*
 * メモリ管理を初期化する
 *
 * @param memsize メモリサイズ(バイト)
 */
void mem_init(u_int memsize)
{
  pm_info.mem_size     = memsize;
  pm_info.blocks_max   = memsize / MEMORY_BLOCK_SIZE;
  pm_info.blocks_used  = 0;
  pm_info.blocks_free  = pm_info.blocks_max;
  pm_info.mem_map      = (u_int*)(__KERNEL_TOP + sizeof_kernel());
  pm_info.mem_map_size = pm_info.blocks_max / 32;
  memset(pm_info.mem_map, 0x00, pm_info.mem_map_size);
  // 0番目は使わせない
  mem_enable_bit(0);
}

/*
 * メモリ上にブロックを確保する
 *
 * @return 確保したブロックのアドレス
 */
void* mem_alloc_block(void)
{
  // ブロックを確保
  u_int frame = mem_find_blocks(1);
  if (frame == 0) return 0;
  // 使用済みフラグを立てる
  mem_enable_bit(frame);
  // アドレスとして返す
  u_int addr = frame * 32;
  return (void*)addr;
}

/*
 * メモリ上のブロックを解放する
 *
 * @param addr mallocしたアドレス
 */
void mem_free_block(void* addr)
{
  u_int frame = (u_int)addr / 32;
  // 使用済みフラグを下げる
  mem_disable_bit(frame);
}

/*
 * 使用可能な連続したメモリブロックを探索する
 *
 * @param size 必要な連続ブロック数
 * @return 確保したブロック番号
 */
u_int mem_find_blocks(u_int size)
{
  u_int i, j, k, ofs;
  if (size == 0) return 0;
  for(i = 0; i < pm_info.mem_map_size; i++) {
    // 1つでも空き(0)がある
    if (pm_info.mem_map[i] != 0xFFFFFFFF) {
      // ビットごとに調べる
      for(j = 0; j < 32; j++) {
	if (!mem_test_bit(j)) {
	  ofs = i * 32 + j;
	  for(k = 0; k <= size; k++) {
	    // 使用不可
	    if (mem_test_bit(ofs + k)) break;
	    // 必要分を確保可能
	    if (k == size) return i * 32 + j;
	  }
	}
      }
    }
  }
  return 0;
}

/*
 * ビットを立てる
 *
 * @param num 何番目のビットを扱うか
 */
void mem_enable_bit(u_int num)
{
  pm_info.blocks_used++;
  pm_info.blocks_free = pm_info.blocks_max = pm_info.blocks_used;
  pm_info.mem_map[num / 32] |= (1 << num);
}

/*
 * ビットを下げる
 *
 * @param num 何番目のビットを扱うか
 */
void mem_disable_bit(u_int num)
{
  pm_info.blocks_used--;
  pm_info.blocks_free = pm_info.blocks_max = pm_info.blocks_used;
  pm_info.mem_map[num / 32] &= ~(1 << num);
}

/*
 * ビットを調べる
 *
 * @param num 何番目のビットを調べるか
 * @return セットされていれば1
 */
u_char mem_test_bit(u_int num)
{
  return (pm_info.mem_map[num / 32] & (1 << num)) > 0;
}

/*
 * カーネル本体のサイズを取得する
 *
 * @return カーネルのサイズ(バイト)
 */
inline u_int sizeof_kernel(void)
{
  return (__KERNEL_BOTTOM - __KERNEL_TOP);
}
