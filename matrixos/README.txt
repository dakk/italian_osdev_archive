----------------------------------
---    Readme di MatrixOS Versione 0.0.20.1
----------------------------------

NOTE SULLA VERSIONE:
	Questa versione a differenza delle precedenti ha un kernel in formato elf e si avvia tramite GRUB (o un'altro bootloader compattibile con le specifiche
	multiboot). La versione di GRUB usata nel floppy è una versione patchata che permette di settare la modalità grafica (e la risoluzione) con cui avviare il sistema. Nelle immagini del Floppy il filesystem usato è minix.



COME COMPILARE ED AVVIARE MATRIXOS

Attenzione: Per compilare MatrixOS è necessario avere un sistema *nix con gcc installato (Non compila con gcc 3.3, consiglio di usare gcc 3.4). Per gli utenti Windows è possibile scrivere l'immagine del Floppy utilizando l'utility rawwritewin.

Per avviare MatrixOS da floppy (compilazione da sorgenti):
	- Aprire una shell nella directory dei sorgenti
	- Inserire un floppy
	- make && make floppy && make fd_install
	- Riavviare il computer
	- Digitare configfile /boot/grub/menu.list al prompt di GRUB
------------------------------
------------------------------
Per avviare MatrixOS da floppy (scrivere l'immagine del floppy da LINUX (o da un'altro sistema *nix) tramite dd):
	- Scaricare l'immagine del floppy da http://matrixos.alproject.org/
	- Digitare dd if=MatrixFloppy of=/dev/fd0 per scrivere l'immagine del floppy.
	- Riavviare il computer.
	- Digitare configfile /boot/grub/menu.list al prompt di GRUB
------------------------------
------------------------------
Per avviare MatrixOS da floppy (scrivere l'immagine del floppy da WINDOWS tramite RawWrite)
	- Scaricare l'immagine del floppy e rawwrite da http://matrixos.alproject.org/
	- Aprire RawWriteWin
	- Selezionare MatrixFloppy
	- Fare click su Write
	- Riavviare il computer
	- Digitare configfile /boot/grub/menu.list al prompt di GRUB
------------------------------
------------------------------
Per avviare MatrixOS da disco.
	- E' sufficiente copiare bin/matrixos nella propria directory /boot.
	- Dopo aver riavviato il computer digitare nel prompt di GRUB (per accedere al prompt digitare c): 
		- kernel /boot/matrixos
		- boot
------------------------------
------------------------------
Per emulare MatrixOS con bochs:
	- Aprire una shell
	- Digitare: bochs -f bochsrc
	- 5 [Invio]
	- Digitare configfile /boot/grub/menu.list al prompt di GRUB
------------------------------
------------------------------
Per emulare MatrixOS con qemu:
	- Aprire una shell
	- Digitare: qemu -fda MatrixFloppy
	- Digitare configfile /boot/grub/menu.list al prompt di GRUB
------------------------------
------------------------------
Per compilare MatrixOS:
	- Aprire una shell
	- Digitare make [Invio]

SEGNALAZIONE BUG:

windowsuninstall@alproject.org
