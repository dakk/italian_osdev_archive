export MAKEFLAGS += --no-print-directory --silent

KERNEL=kernel.bin
MAPPA=kernel.map

export ASM=nasm
export CC=gcc
export CXX=g++
export LIB=$(PWD)/include
export ALIB=-I$(LIB)/hardware -I$(LIB)/libc  -I$(LIB)/cpu -I$(LIB)/mm  -I$(LIB)/  -I$(LIB)/lib -I$(LIB)/libc++ -I$(LIB)/task -I$(LIB)/sys -I$(LIB)/dispositivi -I$(LIB)/app -I$(LIB)/fpu_emu -I$(LIB)/fs
export CFLAG=-fomit-frame-pointer -O $(ALIB)  -Wall  -nostdlib  -nostdinc -fno-builtin
export CXXFLAG=-fomit-frame-pointer -O $(ALIB)  -Wall  -nostdlib   -nostdinc -fno-builtin -fno-exceptions -fno-rtti
export AR=ar
export ARFLAGS=rv
export REMOVE=rm -f

DIRS=main hardware libc cpu mm lib task dispositivi fs fpu_emu
FILES = $(foreach dir,$(DIRS), $(wildcard $(dir)/*.cc))
FILES += $(foreach dir,$(DIRS), $(wildcard $(dir)/*.c))
FILES += $(foreach dir,$(DIRS), $(wildcard $(dir)/*.S))
#CXXFILES = $(FILES:.cc=.o)
CXXFILES=$(patsubst %.cc, %.o, $(foreach dir,$(DIRS), $(wildcard $(dir)/*.cc)))
CFILES = $(patsubst %.c, %.o, $(foreach dir,$(DIRS), $(wildcard $(dir)/*.c)))
AFILES =  $(patsubst %.S, %.o, $(foreach dir,$(DIRS), $(wildcard $(dir)/*.S)))
OFILES =  $(CXXFILES) $(CFILES) $(AFILES)
SCXXFILES=$(foreach dir,$(DIRS), $(wildcard $(dir)/*.cc))
SCFILES=$(foreach dir,$(DIRS), $(wildcard $(dir)/*.c))
DFILES=$(SCXXFILES) $(SCFILES)
INUTILI=$(foreach dir,$(DIRS), $(wildcard $(dir)/*~))

INDENT=indent -br   -sc -ce -cdw -npcs -nbc  -brs -nbap -nbfda -nbad -nip -npsl -i8



export FILE_SYSTEM=-tmsdos
export MONT=mount
export UMONT=umount

export DEVICE=a:
#export DEVICE=b:


#dispositivo dove installare
	export FLOPPY=/dev/fd0
	export MOUNT_FLOPPY=/mnt/floppy
	export OPT=
#	export FLOPPY=$(PWD)/disko.img
	#export MOUNT_FLOPPY=/mnt/floppy
	#export OPT=-o loop


export MONTA=$(MONT) $(OPT) $(FILE_SYSTEM) $(FLOPPY) $(MOUNT_FLOPPY)
export SMONTA=umount $(MOUNT_FLOPPY)

FLOPPYIMG=boot_image.img
BOOTIMAG=boot/bootsec
.c.o:
	@echo "COMPILO" $<
	$(CC)  $(CFLAG) -c -o $@ $<
.cc.o:
	@echo "COMPILO" $<
	$(CXX)  $(CXXFLAG) -c -o $@ $<
.cc.S:
	$(CXX)  $(CXXFLAG) -S -o $@ $<
.S.o:
	@echo "ASSEMBLO" $<
	$(AS)  -o $@ $<
.asm.o:
	@echo "ASSEMBLO" $<
	$(ASM)  -o $@ $<



all: $(OFILES)
	@echo Linko tutto e creo il kernel
	$(LD)  -Bstatic  -o$(KERNEL)  $(OFILES) -Map $(MAPPA) -Tlink.ld
	./lunghezza
	cd boot && $(MAKE) header
	cd boot && $(MAKE)

kernel: $(OFILES)
	@echo Linko tutto e creo il kernel
	$(LD)  -Bstatic  -o$(KERNEL)  $(OFILES) -Map $(MAPPA) -Tlink.ld
	

install: all
	cd boot && $(MAKE) install
	mcopy $(KERNEL) $(DEVICE)
	@echo Italios installato in $(FLOPPY)
	@echo ItaliOs e\' pronto per essere usato. Grazie ^_^
# cancella
clean:
	$(REMOVE) $(FLOPPYIMG)
	$(REMOVE) $(KERNEL)
	$(REMOVE) $(MAPPA)
	$(REMOVE) disko.img
	cd boot && $(MAKE) clean
	for file in $(OFILES); do\
		echo CANCELLO $$file;\
		rm -Rf $$file ; \
	done
	for file in $(INUTILI); do\
		echo CANCELLO $$file;\
		rm -Rf $$file ; \
	done
	rm -Rf .dipendenze

tar:
	$(MAKE) clean
	./comprimi

dep:
	rm -Rf .dipendenze
	for file in $(DFILES); do\
		echo CERCO LE DIPENDENZE DI $$file;\
		gcc -MM $(ALIB) $$file >> .dipendenze ; \
	done

indent:
	$(INDENT)  $(SCXXFILES) $(SCFILES)
	
floppy:
	@echo FORMATTAZIONE IN CORSO....
	dd if=/dev/zero of=$(FLOPPY) bs=512 count=2880
	cd boot && $(MAKE) mkfs
	
elf:
	$(LD)  -Bstatic  -o$(KERNEL)  $(OFILES) -Map $(MAPPA) -Tlink.ld
elf_rel:
	$(LD)  -d -r -Bstatic  -o$(KERNEL)  $(OFILES) -Map $(MAPPA) -Tlink.ld
	

boot:
	cd boot && $(MAKE)
	cd boot && $(MAKE) install
	
-include .dipendenze
