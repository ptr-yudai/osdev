INCLUDE_OBJECTS = include/util.o include/io.o
HAL_OBJECTS = hal/hal.o hal/idt.o hal/pic.o hal/pit.o hal/exception.o hal/irq.o hal/irq_bridge.o hal/key.o
LOADER_OBJECTS = loader/loader.o loader/pmode.o
OBJECTS = $(LOADER_OBJECTS) $(HAL_OBJECTS) $(INCLUDE_OBJECTS) kmain.o

# ビルトツール
CC = gcc
AS = nasm
LD = ld

# オプション
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
LDFLAGS = -melf_i386 -T link.ld
ASFLAGS = -f elf32

## OSのisoをつくる
my_os.iso: kernel.elf ./iso/boot/grub/menu.lst ./iso/boot/grub/stage2_eltorito
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o my_os.iso iso

## kernel.elfをつくる
kernel.elf: link.ld $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

## CとASMをビルド
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

## 実行する
run:
	qemu-system-i386 -boot d -cdrom my_os.iso -m 512

# 削除する
clean:
	rm -rf *.o *~
	rm -rf ./include/*.o ./include/*~
	rm -rf ./hal/*.o ./hal/*~
	rm -rf ./loader/*.o ./loader/*~
	rm -rf kernel.elf my_os.iso
	rm -rf ./iso/boot/kernel.elf
