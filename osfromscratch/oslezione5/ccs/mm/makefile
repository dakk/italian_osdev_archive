LIBNAME=libmm.a

include ../../make.regole

# Gli oggetti da compilare per creare il kernel
OBJS = kmmu.o fis_mmu.o pagine_libere.o mmu.o pagine_virtuali.o

all : $(OBJS)
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS)

# cancella
clean:
	$(REMOVE) $(OBJS) $(LIBNAME)

