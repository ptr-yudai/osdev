bits 32

section .text
global init_A20

;
; A20を有効化する
;
init_A20:
	cli

	call A20wait		; wait
	mov al, 0xAD		; キーボード無効化
	out 0x64, al

	call A20wait		; wait
	mov al, 0xD0		; アウトプットポート読み込み
	out 0x64, al

	call A20waitd		; wait2
	in al, 0x60		; リードバッファレジスタ読み込み
	push eax

	call A20wait
	mov al, 0xD1		; アウトプットポート書き込み
	out 0x64, al
	
	call A20wait
	pop eax
	or al, 0x2		; A20を有効化
	out 0x60, al

	call A20wait
	mov al, 0xAE		; キーボード有効
	out 0x64, al

	call A20wait
	sti
	ret
	
A20wait:
	in al, 0x64
	test al, 2		; コマンド処理完了までループ
	jnz A20wait
	ret

A20waitd:
	in al, 0x64
	test al, 1		; 書き込み処理完了までループ
	jz A20waitd
	ret
