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
global kexit
extern kmain

;; GDTをロード
init_pmode:
	cli
	; gdtrにgdt_tocのアドレスをセット
	mov eax, gdt_toc
	lgdt [eax]
	; 保護モードビットを1にする
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	; csを使おう
	jmp dword 08h:start_pmode

;; 32ビット保護モードへ移行
bits 32
start_pmode:
	mov ax, 0x10
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ds, ax
	pop ebx
	pop eax
	mov esp, 0x90000
	;; カーネルを呼出
	sti
	push eax
	push ebx
	call kmain
kexit:
	hlt
	jmp kexit
