CFLAGS = -nostdlib -fomit-frame-pointer -fno-builtin -Wall -march=i386 -I./header -I./header/libc
OBJ = kernel.o processore/gdt.o processore/irq.o processore/idt.o processore/cpuid.o \
			hw/8259.o hw/8253.o hw/keyboard.o hw/clock.o hw/dma.o hw/floppy.o hw/apm.o \
			mem/paging.o mem/fismem.o \
			io/io.o \
			libc/stdlib.o libc/bitops.o libc/stdio.o libc/string.o \
			task/task.o task/scheduler.o task/tss.o task/semaphore.o task/ipc/signal.o \
			system/shell.o system/commands.o system/syscall.o \
			video.o misc.o libc/ctype.o

dreamos.img: bl.img kernel.bin
	cat bl.img kernel.bin > dreamos.img

bl.img : boot/dreamcatcherdef.asm
	nasm -f bin ./boot/dreamcatcherdef.asm -o bl.img

kernel.bin: $(OBJ)
	ld -Bstatic --oformat binary -okernel.bin $(OBJ) -Ttext 0x10000 -Map kernel.map

kernel.o: kernel.c
video.o: video.c
misc.o: misc.c
processore/gdt.o: processore/gdt.c
processore/idt.o: processore/idt.c
processore/irq.o: processore/irq.c
processore/cpuid.o: processore/cpuid.c
io/io.o: io/io.c
libc/bitops.o: libc/bitops.c
libc/stdlib.o: libc/stdlib.c
libc/ctype.o: libc/ctype.c
libc/stdio.o: libc/stdio.c
libc/string.o: libc/string.c
hw/8259.o: hw/8259.c
hw/8253.o: hw/8253.c
hw/keyboard.o: hw/keyboard.c
hw/clock.o: hw/clock.c
hw/dma.o: hw/dma.c
hw/floppy.o: hw/floppy.c
hw/apm.o: hw/apm.c
mem/fismem.o: mem/fismem.c
mem/paging.o: mem/paging.c
task/task.o: task/task.c
task/scheduler.o: task/scheduler.c
task/tss.o: task/tss.c
task/semaphore.o: task/semaphore.c
task/ipc/signal.o: task/ipc/signal.c
system/shell.o: system/shell.c
system/commands.o: system/commands.c
system/syscall.o: system/syscall.c

.PHONY: clean install

clean:
	rm -f *.img *.bin *.map
	rm -f $(OBJ)

install: dreamos.img
	dd if=dreamos.img of=/dev/fd0

