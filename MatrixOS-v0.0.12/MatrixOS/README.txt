----------------------------------
---    Readme di MatrixOS Versione 0.0.12
----------------------------------

COME COMPILARE ED ESEGUIRE MATRIXOS

Attenzione: Per compilare MatrixOS è necessario avere un sistema GNU/Linux con nasm e gcc installati. Per gli utenti Windows è possibile scrivere l'immagine del Floppy utilizando l'utility rawwritewin presente in tools.

Per eseguire MatrixOS da floppy (compilazione tramite sorgenti):
	- Aprire una shell nella directory dei sorgenti
	- Inserire un floppy
	- Digitare ./BIT [Invio]
	- Riavviare il computer
------------------------------
------------------------------
Per eseguire MatrixOS da floppy (scrivere l'immagine del floppy da LINUX tramite dd):
	- Aprire una shell nella directory Floppy0.0.12
	- Digitare dd if=/dev/full of=/dev/fd0 per cancellare il floppy e per verificare che non ha settori danneggiati.

	Modalità testuale:
		- Digitare dd if=MatrixOS_Floppy.img of=/dev/fd0 per scrivere l'immagine del floppy della versione testuale.

	Modalità grafica:
		- Digitare dd if=MatrixOS_Floppy_VBE.img of=/dev/fd0 per scrivere l'immagine del floppy della versione grafica.

	- Riavviare il computer.
------------------------------
------------------------------
Per eseguire MatrixOS da floppy (scrivere l'immagine del floppy da WINDOWS tramite RawWrite)
	- Aprire RawWriteWin

	- Modalità testuale:
		- Selezionare MatrixOS_Floppy.img
	
	- Modalità grafica:
		- Selezionare MatrixOS_Floppy_VBE.img

	- Fare click su Write
	- Riavviare il computer
------------------------------
------------------------------
Per eseguire MatrixOS con BOCHS:
	- Aprire una shell
	- Digitare ./BITB [Invio]
	- 5 [Invio]
------------------------------
------------------------------
Per compilare MatrixOS:
	- Aprire una shell
	- Digitare ./build [Invio]

SEGNALAZIONE BUG:

matrixos@altervista.it