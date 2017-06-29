#ifndef HAL_MEMORY_H
#define HAL_MEMORY_H

#include "../include/types.h"

/*----- 定数 -----*/
#define MEMORY_KERNEL_START 0x00100000
#define MEMORY_BLOCK_SIZE   4096    // 4KB

/*----- 構造体 -----*/
// ビットマップ情報
typedef struct {
  u_int mem_size;
  u_int blocks_max;
  u_int blocks_used;
  u_int blocks_free;
  u_int *mem_map;
  u_int mem_map_size;
} PHYSICAL_MEMORY_INFO;

/*----- 関数定義 -----*/
void mem_init(u_int memsize);
u_int mem_find_blocks(u_int size);
void* mem_alloc_blocks(u_int size);
void mem_free_blocks(void* addr, u_int size);
void mem_enable_bit(u_int num);
void mem_disable_bit(u_int num);
u_char mem_test_bit(u_int num);
inline u_int sizeof_kernel(void);

/*----- 変数 -----*/
PHYSICAL_MEMORY_INFO pm_info;

#endif
