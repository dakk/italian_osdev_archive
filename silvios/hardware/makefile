LIBNAME=libhardware.a

# Gli oggetti da compilare per creare il kernel
OBJS = io.o 8259.o keyboard.o speaker.o  hdd.o dma.o fdc.o parallel.o printer.o keymap/it.o  vga.o rtc.o video.o serial.o gdt.o register.o pit.o idt.o

all : $(OBJS)
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS)  > /dev/null

# Compilo gli altri
.c.o:
	$(CC)  $(CFLAG) -c -o $@ $<

# cancella
clean:
	cd audio && make clean
	$(REMOVE) $(OBJS) $(LIBNAME)

indent:
	cd audio && make indent
	$(INDENT) *.c
	rm *~

indentluca:
	cd audio && make indentluca
	$(INDENTLUCA) *.c
	rm *~
