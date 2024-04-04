LIBNAME=libhardware.a

include ../../make.regole

# Gli oggetti da compilare per creare il kernel
OBJS = video.o 8259.o irq.o

all : $(OBJS)
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS)

# cancella
clean:
	$(REMOVE) $(OBJS) $(LIBNAME)

