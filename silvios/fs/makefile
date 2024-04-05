LIBNAME=libfs.a

# Gli oggetti da compilare per creare il kernel
OBJS = fs.o dir.o file.o entry.o fopen.o fio.o
all : $(OBJS)
	gcc format.c  -I../include/hardware -o format
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS) > /dev/null

# Compilo gli altri
.c.o:
	$(CC)  $(CFLAG) -c -o $@ $<

# cancella
clean:
	$(REMOVE) $(OBJS) $(LIBNAME) format fs.disk  len_kernel.txt

indent:
	$(INDENT) *.c
	rm *~

indentluca:
	$(INDENTLUCA) *.c
	rm *~
