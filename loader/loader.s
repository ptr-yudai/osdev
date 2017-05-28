global loader
	
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

bits 32

;;; textセクションの開始
section .text
align 4
	; cdromのためのヘッダ
	dd MAGIC_NUMBER
	dd FLAGS
	dd CHECKSUM

;; エントリーポイント
loader:
	mov esp, kstack + KERNEL_STACK_SIZE
	push eax
	push ebx
	; 保護モードに移行
	jmp init_pmode

;;; bssセクションの開始
section .bss
align 32
kstack:
	resb KERNEL_STACK_SIZE
