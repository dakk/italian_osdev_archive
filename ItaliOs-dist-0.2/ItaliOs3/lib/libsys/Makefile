LIB=/usr/local/include/italios
CFLAGS=-fomit-frame-pointer -O -I$(LIB)  -Wall  -nostdlib  -nostdinc -fno-builtin
CDIR=$(basename $(PWD))
NAME=`date '+%Y_%m_%d'-%H_%M_%S`

CFILES = $(patsubst %.c, %.o,$(wildcard *.c))
AFILES =  $(patsubst %.S, %.o, $(wildcard *.S))

OFILES = $(CFILES) $(AFILES)

.c.o:
		@echo "COMPILO" $<
		$(CC)  $(CFLAGS) -c -o $@ $<
		
.S.o:
	@echo "ASSEMBLO" $<
	$(AS)  -o $@ $<

all: $(OFILES)
		cp /usr/local/share/italios/link.ld .
		@echo Linko tutto e creo il programma
		$(LD)  -r -Bstatic  -olibsys.lad $(OFILES)  -T/usr/local/share/italios/link.ld

clean:
	rm -f *.o
	rm -f *~
	rm -f libsys.lad

dist:
	make clean
	tar czvf ../$(NOME)-$(VERSIONE).tar.gz $(CDIR)
	cd $(CDIR)
