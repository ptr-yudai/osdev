#include "idt.h"
#include "hal.h"
#include "exception.h"
#include "irq.h"
#include "../include/io.h"
#include "../include/util.h"

/*
 * IDTを初期化する
 */
void idt_init()
{
  int i;
  // IDTRを設定
  idtr.size = IDT_MAX_INTERRUPTS * sizeof(IDT_DESC);
  idtr.base = (IDT_DESC*)&idt[0];
  memset((void*)&idt[0], 0, IDT_MAX_INTERRUPTS * sizeof(IDT_DESC));
  // デフォルトハンドラを設定
  for(i = 0; i < IDT_MAX_INTERRUPTS; i++) {
    idt_setup_ir(i, idt_default_handler);
  }
  // IDTRにIDTをロード
  idt_load();
  // 用意済みの割り込みハンドラについては設定
  idt_sethandler();
}

/*
 * 用意したハンドラを設定する
 */
void idt_sethandler()
{
  enable_interrupt();
  // 例外ハンドラ
  idt_setup_ir(0, handler_divided_by_zero);
  idt_setup_ir(1, handler_signal_step);
  idt_setup_ir(2, handler_nmi);
  idt_setup_ir(3, handler_breakpoint);
  idt_setup_ir(4, handler_overflow);
  idt_setup_ir(5, handler_bounds_check);
  idt_setup_ir(6, handler_invalid_opcode);
  idt_setup_ir(7, handler_no_device);
  idt_setup_ir(8, handler_double_fault);
  idt_setup_ir(10, handler_invalid_tss);
  idt_setup_ir(11, handler_no_segment);
  idt_setup_ir(12, handler_stack_fault);
  idt_setup_ir(13, handler_general_protection_fault);
  idt_setup_ir(14, handler_page_fault);
  idt_setup_ir(16, handler_fpu_fault);
  idt_setup_ir(17, handler_alignment_check);
  idt_setup_ir(18, handler_machine_check);
  idt_setup_ir(19, handler_simd_fpu_fault);
  
  fb_print("[DEBUG] Set IDT handlers\n");
}

/*
 * 割り込みベクタにハンドラを設定する
 *
 * @param i       割り込みベクタテーブルの何番目か
 * @param handler ハンドラのアドレス
 */
void idt_setup_ir(u_int i, void* handler)
{
  // ベクタテーブルの範囲外
  if (i > IDT_MAX_INTERRUPTS) return;
  // ハンドラがNULLポインタ
  if (!handler) return;
  // idtディスクリプタを設定
  u_int uiBase = (u_int)handler;
  idt[i].baseLo   = (u_short)(uiBase & 0xFFFF);
  idt[i].sel      = IDT_INT_SELECTOR;
  idt[i].reserved = 0;
  idt[i].flags    = (u_char)(IDT_FLAGS_PRESENT | IDT_FLAGS_INTGATE_32BIT);
  idt[i].baseHi   = (u_short)((uiBase >> 16) & 0xFFFF);
}

/*
 * IDTRにIDTをロードする
 */
void idt_load()
{
  __asm__ __volatile__ ("lidt idtr");
}

/*
 * デフォルトハンドラ
 */
void idt_default_handler()
{
  kernel_panic("Unhandled Exception");
}

/*
 * 割り込みを発生する
 *
 * @param sys 割り込み番号
 */
void idt_genint(u_char sys)
{
  asm volatile
    (
     "movb %0, genint+1\n"
     "genint:\n"
     "int $0\n"
     : /*output*/ : "r"(sys) /* input */ : /* clobbered */
     );
}

