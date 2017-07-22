;;; div64.s
;;;  64ビットの除算関係

;; C側にエクスポートする関数
global __do_div64

;; do_div64 - 64bit整数を32bit整数で割った商を求める
;; @param
;;  esi : dividendのポインタ
;;  edi : divisorのポインタ
;; @return
;;  eax : 演算結果の上位32ビット
;;  edx : 演算結果の下位32ビット
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
