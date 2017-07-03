/*
 * pte.c - ページングテーブルエントリー
 */
#include "pte.h"

/*
 * 属性を追加する
 *
 * @param entry     PTEへのポインタ
 * @param attribute 属性
 */
inline void pte_add_attribute(PT_ENTRY* entry, u_int attribute)
{
  *entry |= attribute;
}

/*
 * 属性を破棄する
 *
 * @param entry     PTEへのポインタ
 * @param attribute 属性
 */
inline void pte_del_attribute(PT_ENTRY* entry, u_int attribute)
{
  *entry &= ~attribute;
}

/*
 * 物理アドレスをセットする
 *
 * @param entry PTEへのポインタ
 * @param addr  ページフレームアドレス
 */
inline void pte_set_frameaddr(PT_ENTRY* entry, u_int addr)
{
  *entry = (*entry & ~PTE_FRAME) | addr;
}

/*
 * 物理アドレスを取得する
 *
 * @param entry PTEへのポインタ
 * @return ページフレームアドレス
 */
inline u_int pte_get_frameaddr(PT_ENTRY* entry)
{
  return (*entry & PTE_FRAME);
}

/*
 * Pフラグを調べる
 *
 * @param entry PTEへのポインタ
 * @return presentなら1
 */
inline u_char pte_is_present(PT_ENTRY* entry)
{
  return ((*entry & PTE_PRESENT) == PTE_PRESENT);
}

/*
 * Wフラグを調べる
 *
 * @param entry PTEへのポインタ
 * @return writableなら1
 */
inline u_char pte_is_writable(PT_ENTRY* entry)
{
  return ((*entry & PTE_WRITABLE) == PTE_WRITABLE);
}
