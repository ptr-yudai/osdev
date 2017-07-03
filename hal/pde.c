/*
 * pde.c - ページングディレクトリエントリー
 */
#include "pde.h"

/*
 * 属性を追加する
 *
 * @param entry     PDEへのポインタ
 * @param attribute 属性
 */
inline void pde_add_attribute(PD_ENTRY* entry, u_int attribute)
{
  *entry |= attribute;
}

/*
 * 属性を破棄する
 *
 * @param entry     PDEへのポインタ
 * @param attribute 属性
 */
inline void pde_del_attribute(PD_ENTRY* entry, u_int attribute)
{
  *entry &= ~attribute;
}

/*
 * 物理アドレスをセットする
 *
 * @param entry PDEへのポインタ
 * @param addr  ページフレームアドレス
 */
inline void pde_set_frameaddr(PD_ENTRY* entry, u_int addr)
{
  *entry = (*entry & ~PDE_FRAME) | addr;
}

/*
 * 物理アドレスを取得する
 *
 * @param entry PTEへのポインタ
 * @return ページフレームアドレス
 */
inline u_int pde_get_frameaddr(PD_ENTRY* entry)
{
  return (*entry & PDE_FRAME);
}

/*
 * Pフラグを調べる
 *
 * @param entry PTEへのポインタ
 * @return presentなら1
 */
inline u_char pde_is_present(PD_ENTRY* entry)
{
  return ((*entry & PDE_PRESENT) == PDE_PRESENT);
}

/*
 * Wフラグを調べる
 *
 * @param entry PTEへのポインタ
 * @return writableなら1
 */
inline u_char pde_is_writable(PD_ENTRY* entry)
{
  return ((*entry & PDE_WRITABLE) == PDE_WRITABLE);
}
