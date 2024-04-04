include ../make.regole

#nome dei dile
BOOTIMG=bootsec
BOOTASM=bootsec.asm

all:
	$(ASM) $(BOOTASM) -o $(BOOTIMG)
	
clean:
	$(REMOVE) $(BOOTIMG) kernel_len.inc mkdosfs fat_header.inc floppy_header

mkfs:
	@echo CREAZIONE DEL FILE SYSTEM IN CORSO.......
	gcc mkdosfs.c -o mkdosfs
	./mkdosfs -F 12 -v $(FLOPPY)
	
header:
	gcc floppy_header.c -o floppy_header
	./floppy_header $(FLOPPY) > fat_header.inc
	
install:
	dd if=$(BOOTIMG) of=$(FLOPPY) conv=notrunc
