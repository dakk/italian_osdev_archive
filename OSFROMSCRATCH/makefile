all:
	gcc -fomit-frame-pointer -Wall -march=i386 -c -o kernel.o kernel.c -Iapp -nostdinc -fno-builtin
	ld -Bstatic --oformat binary -o bin/kernel.bin kernel.o -Ttext 0x10000 -Map kernel.map 
	nasm -o bin/bootloader bootloader.asm

install: all
	cat bin/bootloader bin/kernel.bin > boot_image.img
	dd if="boot_image.img" of="/dev/fd0"


