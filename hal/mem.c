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
  // 単位はキロバイト
  pm_info.mem_size     = memsize;
  pm_info.blocks_max   = memsize * 1024 / MEMORY_BLOCK_SIZE;
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
 * @param size 確保するサイズ
 * @return 確保したブロックのアドレス
 */
void* mem_alloc_blocks(u_int size)
{
  u_int i;
  // ブロックを確保
  u_int frame = mem_find_blocks(size);
  if (frame == 0) return 0;
  // 使用済みフラグを立てる
  for(i = 0; i < size; i++) mem_enable_bit(frame + i);
  // アドレスとして返す
  u_int addr = frame * MEMORY_BLOCK_SIZE;
  // [TODO]この確保方法は一時的なものです
  //u_int addr = (u_int)pm_info.mem_map + pm_info.mem_map_size;
  //addr += frame * MEMORY_BLOCK_SIZE;
  return (void*)addr;
}

/*
 * メモリ上のブロックを解放する
 *
 * @param addr mallocしたアドレス
 * @param size mallocしたサイズ
 */
void mem_free_blocks(void* addr, u_int size)
{
  u_int i;
  u_int frame = (u_int)addr / MEMORY_BLOCK_SIZE;
  //u_int frame = (u_int)addr - (u_int)pm_info.mem_map - pm_info.mem_map_size;
  //frame /= MEMORY_BLOCK_SIZE;
  // 使用済みフラグを下げる
  for(i = 0; i < size; i++) mem_disable_bit(frame + i);
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
	ofs = i * 32 + j;
	if (!mem_test_bit(ofs)) {
	  for(k = 0; k <= size; k++) {
	    // 使用不可
	    if (mem_test_bit(ofs + k)) break;
	    // 必要分を確保可能
	    if (k == size) return ofs;
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
  pm_info.blocks_free = pm_info.blocks_max - pm_info.blocks_used;
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
  pm_info.blocks_free = pm_info.blocks_max - pm_info.blocks_used;
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
