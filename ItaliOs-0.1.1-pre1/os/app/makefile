#compilatore

LIBNAME=libapp.a

# Programma per cancellare i file oggetto e gli eseguibili

# Gli oggetti da compilare per creare il kernel
OBJS = help.o version.o screen.o audio.o tempo.o utility.o fs_app.o tema.o shell.o uart.o app_mouse.o game/Indovina_il_numero.o 

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
	$(INDENT) *.c game/*.c
	rm *~
indentluca:
	$(INDENTLUCA) *.c  game/*.c
	rm *~
