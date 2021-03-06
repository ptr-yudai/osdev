#include "hal.h"
#include "idt.h"
#include "irq.h"
#include "pic.h"
#include "pit.h"
#include "key.h"
#include "mem.h"
#include "vmem.h"
#include "ide.h"

/*
 * 各種初期化を実行する
 */
void hal_init(multiboot_info_t* mbd)
{
  idt_init();  // 割り込みテーブル
  pic_init();  // PIC
  pit_init();  // タイマ
  kb_init();   // キーボード
  mem_init(mbd->mem_upper);  // 物理メモリ
  //vmem_init();
  ide_init();  // ハードディスク
  enable_interrupt();
}

/*
 * 割り込みの終了を受け取る
 */
void __attribute__((__cdecl__)) interrupt_done(u_int inter_n)
{
  if (inter_n > 16) return;
  // 割り込み終了をSLAVEに通知する
  if (inter_n >= 8) {
    pic_sendcmd(PIC_OCW2_MASK_EOI, 1);
  }
  // MASTERには常に通知する
  pic_sendcmd(PIC_OCW2_MASK_EOI, 0);
}

/*
 * 割り込みを有効化する(sti)
 */
void __attribute__((__cdecl__)) enable_interrupt()
{
  __asm__ __volatile__("sti");
}

/*
 * 割り込みを無効化する(cli)
 */
void __attribute__((__cdecl__)) disable_interrupt(void)
{
  __asm__ __volatile__("cli");
}

/*
 * 割り込みの開始宣言
 */
inline void enter_interrupt(void)
{
  __asm__ __volatile__("pushal");
}
/*
 * 割り込みの終了宣言
 */
inline void exit_interrupt(void)
{
  __asm__ __volatile__("popal");
  __asm__ __volatile__("leave");
  __asm__ __volatile__("iret");
}
