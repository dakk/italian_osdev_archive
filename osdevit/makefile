CC=gcc

 CFLAG=-fomit-frame-pointer  -Wall -march=i386

 OBJS=main.o
 
 KERNEL=kernel.bin
 
all:	$(OBJS)
	$(LD)  -Bstatic --oformat binary -o$(KERNEL) $(OBJS) -Ttext 0x10000 -Map kernel.map
	nasm bootloader.asm -o bootloader
	
install: all
	cat bootloader $(KERNEL) > boot_image.img
	dd if=boot_image.img of=/dev/fd0

# cancella
clean:
	rm -f $(OBJS)	bootloader boot_image.img kernel.map kernel.bin
	
.c.o:
	$(CC)  $(CFLAG) -c -o $@ $<