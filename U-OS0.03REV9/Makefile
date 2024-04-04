CFLAGS = -nostdlib -fomit-frame-pointer -fno-builtin -Wall -march=i386 -I./header -I./header/libc
OBJ = kernel.o processore/irq.o processore/idt.o processore/cpuid.o hw/8259.o hw/8253.o  \
      hw/keyboard.o hw/clock.o hw/dma.o hw/floppy.o mem/paging.o mem/fismem.o io/io.o libc/stdlib.o libc/bitops.o libc/stdio.o        \
      libc/string.o task/task.o task/scheduler.o task/tss.o task/semaphore.o task/ipc/signal.o            \
      video.o system/kprg.o system/shell.o

uos.img: bl.img kernel.bin
	cat bl.img kernel.bin > uos.img

bl.img : boot/boot.asm
	nasm -f bin ./boot/boot.asm -o bl.img

kernel.bin: $(OBJ)
	ld -Bstatic --oformat binary -okernel.bin $(OBJ) -Ttext 0x10000 -Map kernel.map

kernel.o: kernel.c
video.o: video.c
processore/idt.o: processore/idt.c
processore/irq.o: processore/irq.c
processore/cpuid.o: processore/cpuid.c
io/io.o: io/io.c
libc/bitops.o: libc/bitops.c
libc/stdlib.o: libc/stdlib.c
libc/stdio.o: libc/stdio.c
libc/string.o: libc/string.c
hw/8259.o: hw/8259.c
hw/8253.o: hw/8253.c
hw/keyboard.o: hw/keyboard.c
hw/clock.o: hw/clock.c
hw/dma.o: hw/dma.c
hw/floppy.o: hw/floppy.c
mem/fismem.o: mem/fismem.c
mem/paging.o: mem/paging.c
task/task.o: task/task.c
task/scheduler.o: task/scheduler.c
task/tss.o: task/tss.c
task/semaphore.o: task/semaphore.c
task/ipc/signal.o: task/ipc/signal.c
system/kprg.o: system/kprg.c
system/shell.o: system/shell.c

.PHONY: clean install

clean:
	rm -f *.img *.bin *.map
	rm -f $(OBJ)

install: uos.img
	dd if=uos.img of=/dev/fd0

