#include "exception.h"
#include "../include/io.h"

/*
 * カーネルパニック画面への移行
 * （この関数が呼び出されるとCPUは完全に停止します）
 */
void kernel_panic(const char* msg)
{
  fb_move_cursor(0, 0);
  fb_setcolor(VGA_COLOR_WHITE, VGA_COLOR_RED);
  //fb_clrscr();
  //fb_move_cursor(0, 33);
  //fb_print("[KERNEL PANIC]");
  //fb_move_cursor(1, 0);
  //fb_print("A fatal problem has been detected! \\(^o^)/\nERROR: ");
  fb_print(msg);
  
  asm volatile
    (
     ".loop:\n"
     "cli\n"
     "hlt\n"
     "jmp .loop\n"
     );
}

/*
 * 例外ハンドラ：
 */
void handler_divided_by_zero()
{
  kernel_panic("Divided By Zero");
}
void handler_signal_step()
{
  kernel_panic("Single Step Trap");
}
void handler_nmi()
{
  kernel_panic("NMI Trap");
}
void handler_breakpoint()
{
  kernel_panic("Breakpoint Trap");
}
void handler_overflow()
{
  kernel_panic("Overflow Trap");
}
void handler_bounds_check()
{
  kernel_panic("Bounds Check Fault");
}
void handler_invalid_opcode()
{
  kernel_panic("Invalid Opcode Fault");
}
void handler_no_device()
{
  kernel_panic("No Device Fault");
}
void handler_double_fault()
{
  kernel_panic("Double Fault");
}
void handler_invalid_tss()
{
  kernel_panic("Invalid TSS Fault");
}
void handler_no_segment()
{
  kernel_panic("No Segment Fault");
}
void handler_stack_fault()
{
  kernel_panic("Stack Fault");
}
void handler_general_protection_fault()
{
  kernel_panic("General Protection Fault");
}
void handler_page_fault()
{
  kernel_panic("Page Fault");
}
void handler_fpu_fault()
{
  kernel_panic("FPU Fault");
}
void handler_alignment_check()
{
  kernel_panic("Alignment Check Fault");
}
void handler_machine_check()
{
  kernel_panic("Machine Check Abort");
}
void handler_simd_fpu_fault()
{
  kernel_panic("SIMD FPU Fault");
}
