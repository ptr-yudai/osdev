#ifndef HAL_PTE_H
#define HAL_PTE_H

#include "../include/types.h"

/*----- 定数定義 -----*/
#define PTE_PRESENT       0x01
#define PTE_WRITABLE      0x02
#define PTE_USER          0x04
#define PTE_WRITETHOUGH   0x08
#define PTE_NOT_CACHEABLE 0x10
#define PTE_ACCESSED      0x20
#define PTE_DIRTY         0x40
#define PTE_PAT           0x80
#define PTE_CPU_GLOBAL    0x100
#define PTE_LV4_GLOBAL    0x200
#define PTE_FRAME         0x7FFFF000

/*----- 構造体定義 -----*/
typedef u_int PT_ENTRY;

/*----- 関数定義 -----*/
inline void pte_add_attribute(PT_ENTRY* entry, u_int attribute);
inline void pte_del_attribute(PT_ENTRY* entry, u_int attribute);
inline void pte_set_frameaddr(PT_ENTRY* entry, u_int addr);
inline u_int pte_get_frameaddr(PT_ENTRY* entry);
inline u_char pte_is_present(PT_ENTRY* entry);
inline u_char pte_is_writable(PT_ENTRY* entry);

#endif
