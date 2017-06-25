bits 32

global GetMemorySize
global GetMemoryMap

;; インポート
extern MemoryMapEntry

section .text

;;
;; メモリサイズを取得する(BIOS)
;;
GetMemorySize:
	push ecx
	push edx
	xor ecx, ecx		; ecx = 0
	xor edx, edx 		; edx = 0
	mov eax, 0xe801
	int 0x15		; <---------------------------- FAULT!
	jc .Perror
	cmp ah, 0x86		; サポートしていない
	je .Perror
	cmp ah, 0x80		; 無効なコマンド
	je .Perror
	jcxz .Qsuccess		; cx=0なら成功
	mov ax, cx		; 自分で格納
	mov bx, dx
	jmp .Qsuccess
.Perror:
	xor ax, ax
	dec ax
	xor bx, bx
.Qsuccess:
	pop edx
	pop ecx
	ret

;;
;; メモリマップを取得する
;;
GetMemoryMap:
	; 最初のエントリを取得する
	pushad
	xor ebx, ebx
	xor bp, bp
	mov edx, 'PAMS'
	mov eax, 0xE820
	mov [es:di + 20], dword 1
	mov ecx, 24		; sizeof(memmap_entry)
	int 0x15		; <---------------------------- FAULT!
	jc short .Perror
	cmp eax, 'PAMS'
	jne short .Perror
	test ebx, ebx
	je short .Perror
	jmp short .Pstart
.Lnext:
	mov edx, 'PAMS'
	mov ecx, 24
	mov eax, 0xe820
	mov [es:di + 20], dword 1
	int 0x15
.Pstart:
	jcxz .Pskip
.notext:
	mov ecx, [es:di + MemMapEntry.len]
	test ecx, ecx
	jne .Padopt
	mov ecx, [es:di + MemMapEntry.len + 4]
	jecxz .Pskip
.Padopt:
	inc bp
	add di, 24
.Pskip:
	cmp ebx, 0
	jne .Lnext
	jmp .Qdone
.Perror:
	stc
.Qdone:
	popad
	ret

struc MemMapEntry
	.base resq 1		; 基底アドレス
	.len  resq 1		; サイズ
	.type resd 1		; タイプ
	.acpi resd 1		; 予約
endstruc
