OUTPUT_FORMAT("elf32-i386");
ENTRY(loader)

SECTIONS {
	 . = 0x00100000;

	 /* 機械語領域 */
	 .text ALIGN (0x1000):
	 {
		*(.text)
	 }

	 /* 読み込み専用データ領域 */
	 .rodata ALIGN (0x1000):
	 {
		*(.rodata*)
	 }

	 /* データ領域 */
	 .data ALIGN (0x1000):
	 {
		*(.data)
	 }

	 /* bss領域等 */
	 .bss ALIGN (0x1000):
	 {
		*(COMMON)
		*(.bss)
	 }
}
