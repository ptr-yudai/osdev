;;; io.cにエクスポートする関数
global inb
global outb

;; inb: 1バイトのデータをI/Oポートから受け取る
;;  [esp + 4] arg1: I/Oポート
inb:
	mov dx, [esp + 4]
	in al, dx
	ret

;; outb: 1バイトのデータをI/Oポートに送る
;;  [esp + 8] arg2: 送信するバイト
;;  [esp + 4] arg1: I/Oポート
outb:
	mov al, [esp + 8]
	mov dx, [esp + 4]
	out dx, al
	ret
