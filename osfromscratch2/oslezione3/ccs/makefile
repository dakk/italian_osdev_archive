CCS=ccs.bin

	export ASM=nasm
	export AS=as
	export CC=gcc
	export CXX=g++
	export CPU=-DUSED_CPU=i386 -DCPU_FAMILY=IA32
	export LIB=$(PWD)/include
	export ALIB=-I$(LIB)/hardware -I$(LIB)/libc   -I$(LIB)/
	export MFLAG=-mcpu=i386 -D_ITALIOS_ -march=i386 $(CPU)

	export CFLAG=-fomit-frame-pointer -O $(ALIB)  -Wall  -nostdlib  -nostdinc -fno-builtin -fno-exceptions -fno-rtti $(MFLAG)

	export CXXFLAG=-fomit-frame-pointer -O $(ALIB)  -Wall  -nostdlib   -nostdinc -fno-builtin -fno-exceptions -fno-rtti   $(MFLAG)

	export INDENT=indent -br   -sc -ce -cdw -npcs -nbc  -brs -nbap -nbfda -nbad -nip -npsl -i8

	export AR=ar

	export ARFLAGS=rv


# Programma per cancellare i file oggetto e gli eseguibili
	export REMOVE=rm -f



all:
	#entro in ogni cartella ed eseguo i makefile interni
	#make clean
	cd main && $(MAKE)
	cd hardware && $(MAKE)
	$(LD)  -Bstatic  -o$(CCS) -L./main -L./hardware --whole-archive -t -lmain -lhardware -Map kernel.map -Tlink.ld

# cancella
clean:
	$(REMOVE) $(FLOPPYIMG)
	cd main && $(MAKE) clean
	cd hardware && $(MAKE) clean
