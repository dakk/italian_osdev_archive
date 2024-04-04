KERNEL=kernel/kernel.bin
CCS=ccs/ccs.bin

	export ASM=nasm
	export REMOVE=rm -f

#dispositivo dove installare
FLOPPY=/dev/fd0

FLOPPYIMG=boot_image.img
BOOTIMAG=boot/bootsec


all:
	#entro in ogni cartella ed eseguo i makefile interni
	cd ccs && $(MAKE)
	./lunghezza
	cd boot && $(MAKE)
	cat $(BOOTIMAG) $(CCS) $(KERNEL)  > $(FLOPPYIMG)

install: all
	dd if=$(FLOPPYIMG) of=$(FLOPPY)

# cancella
clean:
	$(REMOVE) $(FLOPPYIMG)
	cd ccs && $(MAKE) clean
	cd boot && $(MAKE) clean

tar:
	$(MAKE) clean
	./comprimi
