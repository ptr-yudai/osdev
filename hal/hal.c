#include "hal.h"
#include "pic.h"

/*
 * 割り込みの終了を受け取る
 */
void __attribute__((__cdecl__)) interrupt_done(u_int inter_n)
{
  if (inter_n > 16) return;
  // 割り込み終了を2番目のPICに通知する
  if (inter_n >= 8) {
    pic_sendcmd(PIC_OCW2_MASK_EOI, 1);
  }
  // 1番目のPICには常に通知する
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
void __attribute__((__cdecl__)) disable_interrupt()
{
  __asm__ __volatile__("cli");
}
