include program.inc

LIB=/usr/local/include/italios
CFLAGS=-fomit-frame-pointer -O -I$(LIB)  -Wall  -nostdlib  -nostdinc -fno-builtin
CDIR=$(basename $(PWD))
NAME=`date '+%Y_%m_%d'-%H_%M_%S`


OFILES = $(patsubst %.c,%.o,$(wildcard *.c))

.c.o:
		@echo "COMPILO" $<
		$(CC)  $(CFLAGS) -c -o $@ $<


all: $(OFILES)
		cp /usr/local/share/italios/start.d .
		cp /usr/local/share/italios/link.ld .
		@echo Linko tutto e creo il programma
		cp /usr/local/share/italios/start.d start.c
		$(CC)  $(CFLAGS) -c start.c -o start.o
		$(LD)  -r -Bstatic  -o$(PROGRAMMA)  start.o $(OFILES) -T/usr/local/share/italios/link.ld
		rm start.c
		rm link.ld

clean:
	rm -f *.o
	rm -f *~
	rm -f $(PROGRAMMA)

dist:
	make clean
	tar czvf ../$(NOME)-$(VERSIONE).tar.gz $(CDIR)
	cd $(CDIR)
