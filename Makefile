INCLUDE_OBJECTS = include/util.o include/io.o include/time.o include/div64.o include/sound.o
HAL_OBJECTS = hal/hal.o hal/idt.o hal/pic.o hal/pit.o hal/exception.o hal/irq.o hal/irq_bridge.o hal/key.o hal/mem.o hal/ide.o hal/vmem.o hal/pte.o hal/pde.o
SYS_OBJECTS = sys/screen.o sys/shell.o
FS_OBJECTS = fs/mbr.o fs/ntfs/ntfs.o fs/ntfs/ntfs_scanner.o fs/ntfs/mmls.o fs/ntfs/icat.o fs/ntfs/fls.o fs/ntfs/cd.o fs/ntfs/timeline.o fs/ntfs/istat.o fs/ntfs/parselog.o fs/ntfs/carving.o
LOADER_OBJECTS = loader/loader.o loader/pmode.o loader/a20.o
OBJECTS = $(LOADER_OBJECTS) $(HAL_OBJECTS) $(SYS_OBJECTS) $(FS_OBJECTS) $(INCLUDE_OBJECTS) kmain.o

# ビルトツール
CC = gcc
AS = nasm
LD = ld

# オプション
CFLAGS = -m32 -nostdlib -nostdinc -fno-tree-scev-cprop -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
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
	$(CC) $(CINCLUDE) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

## 実行する
run:
#	qemu-system-i386 -boot d -cdrom my_os.iso -m 32M -hda hdd/ntfs-img-kw-1.dd
#	qemu-system-i386 -boot d -cdrom my_os.iso -m 128M -hda hdd/dfr-01-ntfs.dd
	qemu-system-i386 -boot d -cdrom my_os.iso -m 128M -hda hdd/von.img

## デバッグする
debug:
	objcopy --only-keep-debug kernel.elf kernel.sym
	objcopy --strip-debug kernel.elf
	objcopy -O binary kernel.elf kernel.bin
	objdump -S -M intel kernel.elf > objdump.txt
	qemu-system-i386 -s -S -boot d -cdrom my_os.iso -m 512M

# 削除する
clean:
	rm -rf *.o *~
	rm -rf ./include/*.o ./include/*~
	rm -rf ./hal/*.o ./hal/*~
	rm -rf ./sys/*.o ./sys/*~
	rm -rf ./fs/*.o ./fs/*~ ./fs/ntfs/*.o ./fs/ntfs/*~
	rm -rf ./loader/*.o ./loader/*~
	rm -rf kernel.* my_os.iso objdump.txt
	rm -rf ./iso/boot/kernel.elf
