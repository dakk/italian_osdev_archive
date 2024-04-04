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
		cp /usr/local/share/italios/libc.d .
		cp /usr/local/share/italios/start.d .
		cp /usr/local/share/italios/link.ld .
		@echo Linko tutto e creo il programma
		mv start.d start.c
		$(CC)  $(CFLAGS) -c start.c -o start.o
		$(LD)  -r -Bstatic  -oall_obj.o start.o  $(OFILES) -Tlink.ld
		mv start.c start.d
		/usr/local/bin/italios/it_func all_obj.o
		mv libc.d libc.c
		$(CC)  $(CFLAGS) -c libc.c -o libc.o
		mv libc.c libc.d
		$(LD)   -Bstatic  -o$(PROGRAMMA)  start.o libc.o $(OFILES) -Tlink.ld
		$(LD)  -r -Bstatic  -o$(PROGRAMMA)  start.o libc.o $(OFILES) -Tlink.ld
		rm libc.d
		rm start.d
		rm link.ld

clean:
	rm -f *.o
	rm -f config_func.h
	rm -f *~
	rm -f $(PROGRAMMA)

dist:
	make clean
	tar czvf ../$(NOME)-$(VERSIONE).tar.gz $(CDIR)
	cd $(CDIR)
