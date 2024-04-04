# Makefile per il kernel core di Exabyte

PATH_TO_TOP = .
OBJS = kprint.o panic.o
MODS = arch/arch.o var/var.o mm/mm.o task/task.o
EXEC = exakernel

.EXPORT_ALL_VARIABLES:
ARCH = ia32

include $(PATH_TO_TOP)/make.rules

.PHONY: all
all: $(EXEC)

.PHONY: modules
modules: arch
	make -C arch
	make -C var
	make -C mm
	make -C task

$(EXEC): arch $(OBJS) modules
	$(LD) -nostdlib -nostartfiles -nodefaultlibs -o $@ -T kernel.ld $(MODS) $(OBJS) -Map $@.map
	$(DISASM) $@ >$@.dis

.PHONY: clean
clean:
	make -C arch clean
	make -C var clean
	make -C mm clean
	make -C task clean
	$(RM) $(RMFLAGS) $(EXEC)
	$(RM) $(RMFLAGS) $(EXEC).dis
	$(RM) $(RMFLAGS) $(EXEC).map
	$(RM) $(RMFLAGS) $(OBJS)
	$(RM) $(RMFLAGS) include/arch

.PHONY: arch
arch:
	$(RM) $(RMFLAGS) include/arch
	cd include; ln -s arch-$(ARCH) arch
