ADESSO=$(shell date "+%d-%m-%Y_%H-%M-%S")
ADESSO2=$(shell date "+%d-%m-%Y")

all : floppy shell util aiuto

floppy :
	cd tools && gcc -Wall -W -O2 -o mkfatfs mkfatfs.c
	dd if=/dev/zero of=disko.img count=2880
	tools/mkfatfs disko.img kernel/boot/bootsec
	cd kernel && make 
	mcopy kernel/kernel.bin g:
	mmd g:program
shell :
	cd program/shell && make clean && make 
	cd ../../ 
	mcopy program/shell/shell.elf g:
util :
	cd program/util && make clean && make 
	cd ../../ 
	mcopy program/util/*.elf g:/program
aiuto :
	mmd g:/aiuto
	mcopy manuali/fopen.man g:/aiuto/fopen

distro :
	echo "Descrizione Cambiamenti in questa release (ctrl+d per finire):"
	echo -ne "$(ADESSO2): " >>CAMBIAMENTI
	pico CAMBIAMENTI
	find . -name "*.bak" -exec rm \{\} \;
	tar cvf temp.tar makefile CAMBIAMENTI kernel* program* tools* manuali*
	cat temp.tar | gzip -9 > ItaliOs-distmiki-current.tar.gz
	cp ItaliOs-distmiki-current.tar.gz ItaliOs-distmiki-$(ADESSO).tar.gz
	rm -f temp.tar
	
