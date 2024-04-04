# Makefile per il modulo boot ia32 del kernel core di Exabyte

PATH_TO_TOP = ../../..
OBJS = head.o init.o ap_boot.o
EXEC = boot.o

include $(PATH_TO_TOP)/make.rules

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(EXEC)

ap_boot.o: ap_boot.asm
	$(NASM) $(NASMFLAGS) -o $@ $<
