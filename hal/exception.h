#ifndef HAL_EXCEPTION_H
#define HAL_EXCEPTION_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- 関数定義 -----*/
void kernel_panic(const char* msg);

/*----- 割り込みハンドラ定義 -----*/
void handler_divided_by_zero();
void handler_signal_step();
void handler_nmi();
void handler_breakpoint();
void handler_overflow();
void handler_bounds_check();
void handler_invalid_opcode();
void handler_no_device();
void handler_double_fault();
void handler_invalid_tss();
void handler_no_segment();
void handler_stack_fault();
void handler_general_protection_fault();
void handler_page_fault();
void handler_fpu_fault();
void handler_alignment_check();
void handler_machine_check();
void handler_simd_fpu_fault();

#endif
