	CCS=ccs.bin

	export CXX=g++

	export LIB=$(PWD)/include
	export ALIB= -I$(LIB)/


	export CXXFLAG=-fomit-frame-pointer -O $(ALIB)  -Wall  -nostdlib   -nostdinc -fno-builtin -fno-exceptions -fno-rtti -march=i386

	export AR=ar

	export ARFLAGS=rv


# Programma per cancellare i file oggetto e gli eseguibili
	export REMOVE=rm -f

all:
	#entro in ogni cartella ed eseguo i makefile interni
	#make clean
	cd main && $(MAKE)
	$(LD)  -Bstatic --oformat binary  -o$(CCS) -L./main --whole-archive -t -lmain -Ttext 0x10000 -Map kernel.map

# cancella
clean:
	cd main && $(MAKE) clean
	$(REMOVE) $(CCS) kernel.map
