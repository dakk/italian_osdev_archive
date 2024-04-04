ASM = nasm
REMOVE = rm -f


#nome dei dile
BOOTIMG=bootsec
BOOTASM=bootsec.asm

all:
	$(ASM) $(BOOTASM) -o $(BOOTIMG)
	cd ..

clean:
	$(REMOVE) $(BOOTIMG)
