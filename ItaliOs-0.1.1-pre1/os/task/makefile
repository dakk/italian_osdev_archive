
LIBNAME=libtask.a

# Gli oggetti da compilare per creare il kernel
OBJS = task.o

all : $(OBJS)
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
	$(INDENT) *.c
	rm *~