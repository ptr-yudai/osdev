#ifndef HAL_VMEM_H
#define HAL_VMEM_H

#include "../include/types.h"
#include "pte.h"
#include "pde.h"

/*----- 定数定義 -----*/
#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR   1024
#define NUM_PTE         0124
#define PAGE_SIZE       4096
#define PAGE_DIRECTORY_INDEX(x)       (((x) >> 22) & 0x03FF)
#define PAGE_TABLE_INDEX(x)           (((x) >> 12) & 0x03FF)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xFFF)

/*----- 構造体定義 -----*/
typedef struct {
  PT_ENTRY entries[PAGES_PER_TABLE];
} PTable;
typedef struct {
  PT_ENTRY entries[PAGES_PER_DIR];
} PDirectory;

/*----- 関数定義 -----*/
u_char vmem_map_page(void* phys, void* virt);
u_char vmem_switch_pde(PDirectory* pdir);
u_char vmem_alloc_page(PT_ENTRY* entry);
void vmem_free_page(PT_ENTRY* entry);
inline PT_ENTRY* vmem_lookup_pte(PTable* ptable, u_int addr);
inline PD_ENTRY* vmem_lookup_pde(PDirectory* pdir, u_int addr);
inline PDirectory* vmem_get_directory(void);
inline void vmem_flush_tlb(u_int addr);
void vmem_init(void);

/*----- 変数定義 -----*/
PDirectory *current_pd;

#endif
