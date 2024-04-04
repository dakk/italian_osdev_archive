LIBNAME=libmain.a

include ../../make.regole

# Gli oggetti da compilare per creare il kernel
OBJS = main.o

all : $(OBJS)
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS)

# cancella
clean:
	$(REMOVE) $(OBJS) $(LIBNAME)

