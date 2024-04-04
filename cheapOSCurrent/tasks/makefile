

GCC=gcc

GCCFLAGS=  -Wall -fomit-frame-pointer -fno-builtin -nostdinc -c



all : Ashell.bin taskA.bin Calend.bin Mmind.bin	Evasio.bin Cat.bin ttt.bin
	echo Fatto!

disk: Ashell.bin taskA.bin Calend.bin Mmind.bin Evasio.bin Cat.bin ttt.bin
	mcopy  -o Ashell.bin		a:
	mcopy  -o taskA.bin		a:
	mcopy  -o Calend.bin		a:
	mcopy  -o Mmind.bin		a:
	mcopy  -o Evasio.bin		a:
	#mcopy  -o Cat.bin		a:
	mcopy  -o ttt.bin		a:

cheapOSlib.o :
	$(GCC) -O2 -Wall -fno-builtin -c -o cheapOSlib.o cheapOSlib.c



Ashell.bin : Ashell.o cheapOSlib.o
	ld -Ttext=0x0 -static -nostdlib --oformat=binary -o Ashell.bin cheapOSlib.o Ashell.o

Ashell.o :
	$(GCC)  -O2 -Wall -fno-builtin -c -ansi -o Ashell.o Ashell.c

taskA.bin : taskA.o cheapOSlib.o
	ld -Ttext 0x0 -static -nostdlib --oformat=binary -o taskA.bin cheapOSlib.o taskA.o

taskA.o :
	$(GCC)  -O2 -Wall -fno-builtin -ansi -c -o taskA.o taskA.c

Calend.bin : Calend.o cheapOSlib.o
	ld -Ttext=0x0 -static -nostdlib --oformat=binary -o Calend.bin cheapOSlib.o Calend.o

Calend.o :
	$(GCC)   -O2 -Wall -fno-builtin -c -o Calend.o Calend.c

Mmind.bin : Mmind.o cheapOSlib.o
	ld -Ttext=0x0 -static -nostdlib --oformat=binary -o Mmind.bin cheapOSlib.o Mmind.o

Mmind.o :
	$(GCC)   -O2 -Wall -fno-builtin -c -o Mmind.o Mmind.c

Evasio.bin : Evasio.o cheapOSlib.o
	ld -Ttext=0x0 -static -nostdlib --oformat=binary -o Evasio.bin cheapOSlib.o Evasio.o

Evasio.o :
	$(GCC)   -O2 -Wall -fno-builtin -c -o Evasio.o Evasio.c

Cat.bin : Cat.o cheapOSlib.o
	ld -Ttext=0x0 -static -nostdlib --oformat=binary -o Cat.bin cheapOSlib.o Cat.o

Cat.o :
	$(GCC)   -O2 -Wall -fno-builtin -c -o Cat.o Cat.c

ttt.bin : ttt.o cheapOSlib.o
	ld -Ttext=0x0 -static -nostdlib --oformat=binary -o ttt.bin cheapOSlib.o ttt.o

ttt.o :
	$(GCC)   -O2 -Wall -fno-builtin -c -o ttt.o ttt.c


.PHONY : clean

clean:
	rm -f *.o *.bin
