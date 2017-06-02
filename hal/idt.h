#ifndef HAL_IDT_H
#define HAL_IDT_H

typedef unsigned int u_int;
typedef unsigned long long u_int64;
typedef unsigned short u_short;
typedef unsigned char u_char;

/*----- 定数定義 -----*/
#define IDT_MAX_INTERRUPTS 256

/*----- 構造体定義 -----*/
/* IDTディスクリプタ */
typedef struct {
  u_short baseLo;
  u_short sel;
  u_char reserved;
  u_char flags;
  u_short baseHi;
}__attribute__((packed)) IDT_DESC;
/* フラグ定義 */
#define	IDT_FLAGS_INTGATE_16BIT 0x06
#define	IDT_FLAGS_TSKGATE       0x05
#define	IDT_FLAGS_CALL_GATE     0x0C
#define	IDT_FLAGS_INTGATE_32BIT 0x0E
#define	IDT_FLAGS_TRPGATE       0x0F
#define	IDT_FLAGS_DPL_LV0       0x00
#define	IDT_FLAGS_DPL_LV1       0x20
#define IDT_FLAGS_DPL_LV2       0x40
#define	IDT_FLAGS_DPL_LV3       0x60
#define	IDT_FLAGS_PRESENT       0x80
#define	IDT_INT_SELECTOR        0x08

/* IDTレジスタ */
typedef struct {
  u_short size;
  IDT_DESC* base;
}__attribute__((packed)) IDTR;

/*----- 関数定義 -----*/
void idt_init();
void idt_load();
void idt_setup_ir(u_int i, void* handler);
void idt_default_handler();
void idt_genint(u_char sys);
void idt_sethandler();

/*----- グローバル変数 -----*/
IDT_DESC idt[IDT_MAX_INTERRUPTS];
IDTR idtr;

#endif

