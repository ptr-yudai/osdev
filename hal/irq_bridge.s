;;; irq_bridge.s
;;;  IRQをCに書くとスタックが壊れるので
;;;  アセンブリから呼びだす

;; C側にエクスポートするブリッジ
global irq_pit_bridge
global irq_keyboard_bridge

;; IRQのハンドラ本体(C側)
extern irq_pit
extern irq_keyboard
	
;; PIT(IRQ0)
irq_pit_bridge:
	pushad
	call irq_pit
	popad
	iret

;; キーボード(IRQ1)
irq_keyboard_bridge:
	pushad
	cli
	call irq_keyboard
	sti
	popad
	iret
