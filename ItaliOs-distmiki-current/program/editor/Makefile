LIB=../../tools
CFLAGS=-fomit-frame-pointer -O -I$(LIB)  -Wall  -nostdlib  -nostdinc -fno-builtin
CDIR=$(basename $(PWD))
NAME=`date '+%Y_%m_%d'-%H_%M_%S`
INCLUDE=-I$(LIB)/include

OFILES = $(patsubst %.c,%.o,$(wildcard *.c))
.c.o:
		@echo "COMPILO" $<
		$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

evvai :
		make tutto -e OBJ="editor.o" ELF="editor.elf"

tutto: $(OFILES)
		cp $(LIB)/libc.d .
		cp $(LIB)/start.d .
		cp $(LIB)/link.ld .
		@echo Linko tutto e creo il programma
		mv start.d start.c
		$(CC) $(INCLUDE) $(CFLAGS) -c start.c -o start.o
		$(LD)  -r -Bstatic  -oall_obj.o start.o $(OBJ) -Tlink.ld
		mv start.c start.d
		$(LIB)/it_func all_obj.o
		mv libc.d libc.c
		$(CC) $(INCLUDE) $(CFLAGS) -c libc.c -o libc.o
		mv libc.c libc.d
		$(LD)   -Bstatic  -o$(ELF)  start.o libc.o $(OBJ) -Tlink.ld
		$(LD)  -r -Bstatic  -o$(ELF)  start.o libc.o $(OBJ) -Tlink.ld
		rm libc.d
		rm start.d
		rm link.ld
all: $(OFILES)
		cp $(LIB)/libc.d .
		cp $(LIB)/start.d .
		cp $(LIB)/link.ld .
		@echo Linko tutto e creo il programma
		mv start.d start.c
		$(CC) $(INCLUDE) $(CFLAGS) -c start.c -o start.o
		$(LD)  -r -Bstatic  -oall_obj.o start.o  $(OFILES) -Tlink.ld
		mv start.c start.d
		$(LIB)/it_func all_obj.o
		mv libc.d libc.c
		$(CC) $(INCLUDE) $(CFLAGS) -c libc.c -o libc.o
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
	rm -f *.elf

dist:
	make clean
	tar czvf ../$(NOME)-$(VERSIONE).tar.gz $(CDIR)
	cd $(CDIR)
