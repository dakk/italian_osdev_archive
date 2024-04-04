LIBNAME=liblibc.a

# Gli oggetti da compilare per creare il kernel
OBJS = string.o stdio.o math.o stdlib.o

all : $(OBJS)
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS) > /dev/null



# Compilo gli altri
.c.o:
	$(CC)  $(CFLAG) -D_ITALIOS_ -c -o $@ $<


# cancella
clean:
	$(REMOVE) $(OBJS) $(LIBNAME)

indent:
	$(INDENT) *.c
	rm *~

indentluca:
	$(INDENTLUCA) *.c
	rm *~
