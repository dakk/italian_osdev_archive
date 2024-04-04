# Programma per cancellare i file oggetto e gli eseguibili
REMOVE = rm -f

#le librerie
LIB = include/

#dispositivo dove installare
FLOPPY=/dev/fd0

# compilatore in C
CC = gcc-2.95.3
CFLAG = -fomit-frame-pointer -O -I$(LIB) -Wall 

# il linker
LD  = ld -s

# Gli oggetti da compilare per creare il kernel
OBJS = idt.o kernel.o io.o video.o 8259.o keyboard.o string.o time.o stdio.o ushell.o mem.o pit.o delay.o speaker.o


# nomne dei file
KERNELFN = kernel.elf
KERNELIMG=boot_image.img
BOOTIMAG=boot/bootsec
BOOTDIR=boot/

all: $(KERNELFN)

$(KERNELFN) : $(OBJS)
	$(LD)  -o $@ $(OBJS)  -Ttext 0x100000
	cd $(BOOTDIR) && make
	cat $(BOOTIMAG) $(KERNELFN) > $(KERNELIMG)

# Compilo gli altri
.c.o:
	$(CC)  $(CFLAG) -c -o $@ $<

install: all
	dd if=$(KERNELIMG) of=$(FLOPPY)

# cancella
clean:
	$(REMOVE) $(OBJS) $(KERNELFN) $(KERNELIMG)
	cd $(BOOTDIR) && make clean
