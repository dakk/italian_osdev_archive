# AleOS makefile					   #
# Copyright (c) 2005 Alessandro presta #

all:
	gcc -fomit-frame-pointer -Wall -march=i386 -c -o src/kernel.o src/kernel.c -Iinclude/sys -Iapp -nostdinc -fno-builtin
	gcc -fomit-frame-pointer -Wall -march=i386 -c -o src/io.o src/io.c -Iinclude -nostdinc -fno-builtin
	gcc -fomit-frame-pointer -Wall -march=i386 -c -o src/math.o src/math.c -Iinclude -nostdinc -fno-builtin
	gcc -fomit-frame-pointer -Wall -march=i386 -c -o app/app.o app/app.c -Iinclude -Iapp -nostdinc -fno-builtin
	ld -Bstatic --oformat binary -o bin/kernel.bin src/kernel.o app/app.o src/io.o src/math.o -Ttext 0x10000 -Map kernel.map 
	nasm -o bin/bootloader src/bootloader.asm

install: all
	cat bin/bootloader bin/kernel.bin > boot_image.img

