# Makefile per i moduli di architettura del kernel core di Exabyte

PATH_TO_TOP = ..
EXEC = arch.o
ARCH = ia32

include $(PATH_TO_TOP)/make.rules

.PHONY: all
all: $(EXEC)
	@echo "Compilazione per architettura $(ARCH)"

# costruisce l'oggetto dell'architettura scelta
$(EXEC): build-arch
	cp -f $(ARCH)/arch-$(ARCH).o $(EXEC)

.PHONY: build-arch
build-arch:
	make -C $(ARCH)
	
.PHONY: clean
clean:
	make -C $(ARCH) clean
	rm -f $(EXEC)
