;;; div64.s
;;;  64ビットの除算関係

;; C側にエクスポートする関数
global __do_div64

;; do_div64
;; @param
;;  esi : dividendのポインタ
;;  edi : divisorのポインタ
__do_div64:
	xor edx, edx
	mov eax, [esi + 4]	; edx:eax
	div dword [edi]		; eax = eax / edi, edx = eax & edi
	mov [esp - 4], eax	; 適当な場所にeaxを保存
	mov eax, [esi]          ; edx:eax
	div dword [edi]         ; eax = eax / edi, edx = eax & edi
	mov [esp - 8], eax	; 適当な場所にeaxを保存
	mov eax, [esp - 8]
	mov edx, [esp - 4]
	ret

