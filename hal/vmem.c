/*
 * vmem.c - 仮想メモリ管理
 */
#include "vmem.h"
#include "mem.h"
#include "../include/util.h"
#include "../include/io.h"

/*
 * 仮想メモリと物理メモリを関連付ける
 *
 * @param phys 物理アドレス
 * @param virt 仮想アドレス
 * @return 成功したら1
 */
u_char vmem_map_page(void* phys, void* virt)
{
  PDirectory* pdir;
  PTable* ptable;
  PD_ENTRY* pde;
  PT_ENTRY* pte;
  // ページディレクトリを取得
  pdir = vmem_get_directory();
  if (!pdir) return 0;
  // PDEを取得
  pde = vmem_lookup_pde(pdir, (u_int)virt);
  // ページテーブルがない
  if (!pde_is_present(pde)) {
    // ページテーブルを割り当てる
    ptable = (PTable*)mem_alloc_blocks(1);
    if (!ptable) return 0;
    // ページテーブルを初期化する
    memset(ptable, 0, sizeof(PTable));
    pde_add_attribute(pde, PDE_PRESENT | PDE_WRITABLE);
    pde_set_frameaddr(pde, (u_int)ptable);
  } else {
    ptable = (PTable*)pde_get_frameaddr(pde);
  }
  pte = vmem_lookup_pte(ptable, (u_int)virt);
  pte_add_attribute(pte, PTE_PRESENT | PTE_WRITABLE);
  pte_set_frameaddr(pte, (u_int)phys);
  return 1;
}

/*
 * ページディレクトリをCR3に書き込む
 *
 * @param pdir ページディレクトリ
 * @return 成功したら1
 */
u_char vmem_switch_pde(PDirectory* pdir)
{
  if (!pdir) return 0;
  current_pd = pdir;
  // CR3に書き込み
  __asm__ __volatile__("mov %0, %%cr3": :"r"(pdir));
  return 1;
}

/*
 * ページを確保する
 *
 * @param entry PTEへのポインタ
 * @return 確保に成功したら1
 */
u_char vmem_alloc_page(PT_ENTRY* entry)
{
  // 1ブロック(=4096byte)割り当て
  void* ptr = mem_alloc_blocks(1);
  if (!ptr) return 0;
  // PTEを設定
  pte_set_frameaddr(entry, (u_int)ptr);
  pte_add_attribute(entry, PTE_PRESENT);
  return 1;
}

/*
 * ページを解放する
 *
 * @param entry PTEへのポインタ
 */
void vmem_free_page(PT_ENTRY* entry)
{
  void* ptr = (void*)pte_get_frameaddr(entry);
  if (!ptr) return;
  pte_del_attribute(entry, PTE_PRESENT);
}

/*
 * PTEのインデクスを取得する
 *
 * @param ptable ページテーブル
 * @param 仮想メモリアドレス
 * @return 対応するPTE
 */
inline PT_ENTRY* vmem_lookup_pte(PTable* ptable, u_int addr)
{
  if (!ptable) return 0;
  return &ptable->entries[PAGE_TABLE_INDEX(addr)];
}

/*
 * PDEのインデクスを取得する
 *
 * @param pdir ページディレクトリ
 * @param 仮想メモリアドレス
 * @return 対応するPDE
 */
inline PD_ENTRY* vmem_lookup_pde(PDirectory* pdir, u_int addr)
{
  if (!pdir) return 0;
  return &pdir->entries[PAGE_DIRECTORY_INDEX(addr)];
}

/*
 * ページディレクトリを取得する
 *
 * @return PDirectoryへのポインタ
 */
inline PDirectory* vmem_get_directory(void)
{
  return (current_pd);
}

/*
 * TLBをフラッシュする
 *
 * @param フラッシュしたい仮想アドレス
 */
inline void vmem_flush_tlb(u_int addr)
{
  __asm__ __volatile__("cli");
  __asm__ __volatile__("invlpg %0": :"m"(addr): "memory");
  __asm__ __volatile__("sti");
}

/*
 * 仮想メモリ管理を初期化する
 */
void vmem_init(void)
{
  int i;
  PTable *table_low, *table_high;
  PT_ENTRY *pte;
  PDirectory *directory;
  PD_ENTRY *pde;
  u_int frame;
  u_int virt;

  // 初期化
  table_low = (PTable*)mem_alloc_blocks(1);
  table_high = (PTable*)mem_alloc_blocks(1);
  if (!table_low || !table_high) return;

  //// PTEを初期化
  memset(table_low, 0, sizeof(PTable));
  memset(table_high, 0, sizeof(PDirectory));
  // 0x00000000からの4MB
  for(i = 0, frame = 0, virt = 0;
      i < NUM_PTE;
      i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
    pte = vmem_lookup_pte(table_low, virt);
    pte_add_attribute(pte, PTE_PRESENT | PTE_WRITABLE);
    pte_set_frameaddr(pte, frame);
  }
  // 0x00100000からの4MB
  for(i = 0, frame = 0x00010000, virt = 0x00100000;
      i < NUM_PTE;
      i++, frame += PAGE_SIZE, virt += PAGE_SIZE) {
    pte = vmem_lookup_pte(table_high, virt);
    pte_add_attribute(pte, PTE_PRESENT | PTE_WRITABLE);
    pte_set_frameaddr(pte, frame);
  }

  //// PDEを初期化
  directory = (PDirectory*)mem_alloc_blocks(1);
  if (!directory) return;
  // 0x00000000 - 0x003ff000
  pde = vmem_lookup_pde(directory, 0x00000000);
  pde_add_attribute(pde, PDE_PRESENT | PDE_WRITABLE);
  pde_set_frameaddr(pde, (u_int)table_low);
  // 0x00100000 - 0x004ff000
  pde = vmem_lookup_pde(directory, 0x00000000);
  pde_add_attribute(pde, PDE_PRESENT | PDE_WRITABLE);
  pde_set_frameaddr(pde, (u_int)table_high);

  // ページディレクトリをロード
  vmem_switch_pde(directory);
  
  // ページングを有効化
  //__asm__ __volatile__("cli");
  __asm__ __volatile__("push %eax");
  __asm__ __volatile__("mov %eax, %cr0");
  __asm__ __volatile__("or %eax, 0x80000000");
  __asm__ __volatile__("mov %cr0, %eax");
  __asm__ __volatile__("pop %eax");

  fb_print("[DEBUG] Paging: ON\n");
}
