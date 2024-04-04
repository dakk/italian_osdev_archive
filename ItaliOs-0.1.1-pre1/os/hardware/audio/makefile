#compilatore
CC=gcc
LIB=../../include
ALIB=-I$(LIB)/libc -I$(LIB)/utils -I$(LIB)/hardware  -I$(LIB)/app -I$(LIB)/audio -I$(LIB)/fs -I$(LIB)/mem -I$(LIB)/aux  -I$(LIB)
CFLAG=-fomit-frame-pointer -O $(ALIB)  -Wall

# Programma per cancellare i file oggetto e gli eseguibili
REMOVE=rm -f

OBJS = dsp.o mixer.o midi.o

# Gli ogdsp.o
all : $(OBJS)



# Compilo gli altri
.c.o:
	$(CC)  $(CFLAG) -c -o $@ $<

# cancella
clean:
	$(REMOVE) $(OBJS) $(LIBNAME)

indent:
	$(INDENT) *.c
	rm *~
indentluca:
	$(INDENTLUCA) *.c
	rm *~
