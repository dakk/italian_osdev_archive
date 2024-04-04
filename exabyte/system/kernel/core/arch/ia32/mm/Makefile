# Makefile per il modulo mm ia32 del kernel core di Exabyte

PATH_TO_TOP = ../../..
OBJS = paging.o
EXEC = mm.o

include $(PATH_TO_TOP)/make.rules

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(EXEC)
