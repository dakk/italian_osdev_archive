
include ../make.regole

#nome dei dile
BOOTIMG=bootsec
BOOTASM=bootsec.asm

all:
	$(ASM) $(BOOTASM) -o $(BOOTIMG)
	
clean:
	$(REMOVE) $(BOOTIMG) kernel_len.inc
