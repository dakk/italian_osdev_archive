LIBNAME=libmem.a

# Gli oggetti da compilare per creare il kernel
OBJS = mem.o

all : $(OBJS)
	nasm -f elf mem.asm -o mema.o
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS) mema.o > /dev/null


# Compilo gli altri
.c.o:
	$(CC)  $(CFLAG) -c -o $@ $<

.asm.o:
	nasm -f elf $<

# cancella
clean:
	$(REMOVE) $(OBJS)  $(LIBNAME) cr3.o


indent:
	$(INDENT) *.c
	rm *~

indentluca:
	$(INDENTLUCA) *.c
	rm *~
