# Makefile per il modulo var del kernel core di Exabyte

PATH_TO_TOP = ..
OBJS = vsprintf.o
EXEC = var.o

include $(PATH_TO_TOP)/make.rules

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(EXEC)
