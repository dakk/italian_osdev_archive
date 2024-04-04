ASM = nasm
#compilatore
CC=gcc
LIB=../include/
CPU=PENTIUM1
CFLAG=-fomit-frame-pointer -O  -I$(LIB) -Wall --pedantic -d_$(CPU)_  -D_ItalianOs_

#linker
LD=ld
LDFLAGS=-s -x

# Programma per cancellare i file oggetto e gli eseguibili
REMOVE=rm -f

#nome dei dile
BOOTIMG=bootsec
BOOTASM=bootsec.asm

all:
	$(ASM) $(BOOTASM) -o $(BOOTIMG)
	
clean:
	$(REMOVE) $(BOOTIMG) kernel_len.inc
