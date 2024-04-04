#compilatore

LIBNAME=libapp.a

# Programma per cancellare i file oggetto e gli eseguibili

# Gli oggetti da compilare per creare il kernel
OBJS = help.o version.o screen.o audio.o tempo.o utility.o fs_app.o tema.o shell.o uart.o game/Indovina_il_numero.o game/The_Any_D_J_DiCe_GaMe_1.0.o

all : $(OBJS)
	#cd game && make
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS)  > /dev/null

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
