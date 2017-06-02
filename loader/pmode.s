bits 32

section .data
align 4

;; GDT本体の定義
gdt_data:
; null descriptor 
	dd 0
	dd 0 
; code descriptor
	dw 0FFFFh 			; limit low
	dw 0 				; base low
	db 0 				; base middle
	db 10011010b 			; access
	db 11001111b 			; granularity
	db 0 				; base high
; data descriptor
	dw 0FFFFh 			; limit low (Same as code)
	dw 0 				; base low
	db 0 				; base middle
	db 10010010b 			; access
	db 11001111b 			; granularity
	db 0				; base high

;; GDT構造体
gdt_toc: 
	dw 8*3			 	; GDTのサイズ
	dd gdt_data 			; GDTの先頭アドレス

section .text
global init_pmode
extern kmain

;; GDTをロード
init_pmode:
	cli
	;; gdtrにgdt_tocのアドレスをセット
	lgdt [gdt_toc]
	;; 保護モードビットを1にする
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	;; csを使おう
	jmp 08h:start_pmode

;; 32ビット保護モードへ移行
start_pmode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x9000
	;; カーネルを呼出
	call kmain
.fin:
	hlt
	jmp .fin
