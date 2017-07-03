#ifndef HAL_PDE_H
#define HAL_PDE_H

#include "../include/types.h"

/*----- 定数定義 -----*/
#define PDE_PRESENT    0x01
#define PDE_WRITABLE   0x02
#define PDE_USER       0x04
#define PDE_PWT        0x08
#define PDE_PCD        0x10
#define PDE_ACCESSED   0x20
#define PDE_DIRTY      0x40
#define PDE_4MB        0x80
#define PDE_CPU_GLOBAL 0x100
#define PDE_LV4_GLOBAL 0x200
#define PDE_FRAME      0x7FFFF000

/*----- 構造体定義 -----*/
typedef u_int PD_ENTRY;

/*----- 関数定義 -----*/
inline void pde_add_attribute(PD_ENTRY* entry, u_int attribute);
inline void pde_del_attribute(PD_ENTRY* entry, u_int attribute);
inline void pde_set_frameaddr(PD_ENTRY* entry, u_int attribute);
inline u_int pde_get_frameaddr(PD_ENTRY* entry);
inline u_char pde_is_present(PD_ENTRY* entry);
inline u_char pde_is_writable(PD_ENTRY* entry);
//inline u_char pde_is_user(PD_ENTRY* entry);
//inline u_char pde_is_4mb(PD_ENTRY* entry);

#endif
