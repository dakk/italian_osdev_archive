# Makefile per il modulo architettura ia32 del kernel core di Exabyte

PATH_TO_TOP = ../..
OBJS = setup.o vga_text.o idt.o gdt.o exc.o irq.o pic8259.o multiboot.o smp.o apic.o msr.o cpuid.o ioapic.o asm.o
MODS = boot/boot.o mm/mm.o
EXEC = arch-ia32.o

include $(PATH_TO_TOP)/make.rules

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS) modules
	$(LD) -r -o $@ $(MODS) $(OBJS)
	
.PHONY: modules
modules:
	make -C boot
	make -C mm

.PHONY: clean
clean:
	make -C boot clean
	make -C mm clean
	rm -f $(OBJS)
	rm -f $(EXEC)
