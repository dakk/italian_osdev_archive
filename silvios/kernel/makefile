LIBNAME=libkernel.a

# Gli oggetti da compilare per creare il kernel
OBJS = kernel.o delay.o

all: $(OBJS)
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS) > /dev/null


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
