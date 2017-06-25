global loader
global MemoryMapEntry
	
;;; 定義たち
;; ブートローダのヘッダ
	MAGIC_NUMBER equ 0x1BADB002    ; マジックナンバー(必ず1 bad boot)
	FLAGS        equ 0x0	       ; マルチブート
	CHECKSUM     equ -MAGIC_NUMBER ; MAGIC + CHECK + FLAG = 0
;; スタック
	KERNEL_STACK_SIZE equ 4096 ; スタックのサイズ(byte)
;; 外部関数
extern init_pmode
extern kmain
extern GetMemorySize
extern GetMemoryMap

;;; textセクションの開始
section .text
align 4
	; cdromのためのヘッダ
	dd MAGIC_NUMBER
	dd FLAGS
	dd CHECKSUM

bits 32
;; エントリーポイント
loader:
	mov esp, kstack + KERNEL_STACK_SIZE
	push esp
	push eax
	push ebx
	; 保護モードに移行
	jmp dword init_pmode

;;; dataセクションの開始
section .data
; メモリマップエントリ
struc MemMapEntry
	.base resq 1		; 基底アドレス
	.len  resq 1		; サイズ
	.type resd 1		; タイプ
	.acpi resd 1		; 予約
endstruc
MemoryMapEntry:
istruc MemMapEntry
at MemMapEntry.base, dd 0
at MemMapEntry.len,  dd 0
at MemMapEntry.type, dd 0
at MemMapEntry.acpi, dd 0
iend

;;; bssセクションの開始
section .bss
align 32
; スタック
kstack:
	resb KERNEL_STACK_SIZE
