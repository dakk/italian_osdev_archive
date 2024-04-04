
all: kernel

kernel:
	cd src/kernel && make kernel

floppy: kernel
	@ echo -n "Monto l'immagine del floppy in loopback... "
	@ mkdir floppyloop
	@ mount MatrixFloppy floppyloop/ -t minix -o loop
	@ echo "Fatto"

	@ echo -n "Copio il kernel: bin/matrixos nell'immagine del floppy... "
	@ cp bin/matrixos floppyloop/boot/matrixos
	@ echo "Fatto"

	@ echo "Smonto l'immagine del floppy... "
	@ umount floppyloop/
	@ rmdir floppyloop
	@ echo "Fatto"

fd_install: floppy
	@ echo "Trasferisco il filesystem sul floppy... "
	@ dd if=MatrixFloppy of=/dev/fd0
	@ echo "Fatto"

dist: clean
	@ echo -n "Creo un archivio per la distribuzione... "
	@ cd .. && tar jcvf MatrixOS/MatrixOS-`date +%Y%m%d`.tar.bz2 MatrixOS > /dev/null
	@ echo "Fatto"

clean: clean-all

clean-all:
	cd src/kernel && make clean