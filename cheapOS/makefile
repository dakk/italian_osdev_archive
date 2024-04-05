
FILES = kernel.o io.o isr.o scheduler.o syscall_task.o filesystem.o syscall_filesystem.o mem.o kconsole.o kmath.o kstring.o cpu.o cpudetect.o \
	      irq.o  time.o cmos.o descrittori.o task.o syscall.o pidcoda.o keycoda.o keyboard.o kbdlayout_ita.o graphics.o floppy.o dma.o fs_kernel.o \
	      fs_fat12.o semaphoro.o fs_console.o kbd_shortcuts.o console.o cache.o syscall_system.o


GCC=gcc

GCCFLAGS=  -Wall -fomit-frame-pointer -fno-builtin -nostdinc -c



all : kernel.elf
	cd tasks && $(MAKE)
	echo Fatto!

disk: kernel.elf
	dd if=grub/grub.img	of=/dev/fd0
	mcopy  -o grub/boot/grub/menu.lst a:/boot/grub/
	mcopy  -o kernel.elf		a:
	cd tasks && $(MAKE) disk



kernel.elf : bootgrub.o $(FILES)
	ld  -Ttext 0x100000 -static -nostdlib  -o kernel.elf bootgrub.o  $(FILES)

kerneldump :   $(FILES)
	ld  -Ttext 0x100000 -static -nostdlib  -o kernel.elf $(FILES)
	objdump -Dlx kernel.elf >kerneldump.asm



bootgrub.o :
	nasm -f elf -o bootgrub.o  bootgrub.asm



kernel.o :
	$(GCC) $(GCCFLAGS) -o kernel.o		kernel.c
kconsole.o :
	$(GCC) $(GCCFLAGS) -o kconsole.o	kconsole.c
io.o :
	$(GCC) $(GCCFLAGS) -o io.o			io.c
mem.o :
	$(GCC) $(GCCFLAGS) -o mem.o		mem.c
kmath.o :
	$(GCC) $(GCCFLAGS) -o kmath.o		kmath.c
kstring.o :
	$(GCC) $(GCCFLAGS) -o kstring.o		kstring.c
cpu.o	:
	$(GCC) $(GCCFLAGS) -o cpu.o		cpu.c
cpudetect.o	:
	$(GCC) $(GCCFLAGS) -o cpudetect.o	cpudetect.c
irq.o	:
	$(GCC) $(GCCFLAGS) -o irq.o		irq.c
isr.o	:
	$(GCC) $(GCCFLAGS) -o isr.o		isr.c
time.o	:
	$(GCC) $(GCCFLAGS) -o time.o	 	time.c
cmos.o	:
	$(GCC) $(GCCFLAGS) -o cmos.o 		cmos.c
descrittori.o :
	$(GCC) $(GCCFLAGS) -o descrittori.o 	descrittori.c
task.o :
	$(GCC) $(GCCFLAGS) -o task.o 		task.c
syscall.o :
	$(GCC) $(GCCFLAGS) -o syscall.o 	syscall.c
scheduler.o :
	$(GCC) $(GCCFLAGS) -o scheduler.o 	scheduler.c
pidcoda.o :
	$(GCC) $(GCCFLAGS) -o pidcoda.o 	pidcoda.c
syscall_task.o :
	$(GCC) $(GCCFLAGS) -o syscall_task.o syscall_task.c
keyboard.o :
	$(GCC) $(GCCFLAGS) -o keyboard.o 	keyboard.c
filesystem.o :
	$(GCC) $(GCCFLAGS) -o filesystem.o 	filesystem.c
syscall_filesystem.o :
	$(GCC) $(GCCFLAGS) -o syscall_filesystem.o syscall_filesystem.c
keycoda.o :
	$(GCC) $(GCCFLAGS) -o keycoda.o 	keycoda.c
kbdlayout_usa.o :
	$(GCC) $(GCCFLAGS) -o kbdlayout_usa.o 	kbdlayout_usa.c
kbdlayout_ita.o :
	$(GCC) $(GCCFLAGS) -o kbdlayout_ita.o 	kbdlayout_ita.c
graphics.o :
	$(GCC) $(GCCFLAGS) -o graphics.o 	graphics.c
dma.o :
	$(GCC) $(GCCFLAGS) -o dma.o 	dma.c
floppy.o :
	$(GCC) $(GCCFLAGS) -o floppy.o	floppy.c
fs_kernel.o :
	$(GCC) $(GCCFLAGS) -o fs_kernel.o	fs_kernel.c
fs_fat12.o :
	$(GCC) $(GCCFLAGS) -o fs_fat12.o	fs_fat12.c
semaphoro.o :
	$(GCC) $(GCCFLAGS) -o semaphoro.o	semaphoro.c
fs_console.o :
	$(GCC) $(GCCFLAGS) -o fs_console.o	fs_console.c
kbd_shortcuts.o :
	$(GCC) $(GCCFLAGS) -o kbd_shortcuts.o	kbd_shortcuts.c
console.o :
	$(GCC) $(GCCFLAGS) -o console.o	console.c
cache.o :
	$(GCC) $(GCCFLAGS) -o cache.o	cache.c
syscall_system.o :
	$(GCC) $(GCCFLAGS) -o syscall_system.o	syscall_system.c



.PHONY : clean

clean:
	rm -f *.o *.elf *.bin kerneldump.asm root.img
	cd tasks && $(MAKE) clean
