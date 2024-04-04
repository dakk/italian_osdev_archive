AS=nasm
CC=gcc
BCC=bcc


INITFLAGS=-I$(LIB_INCLUDE_DIR) -I$(DRIVER_INCLUDE_DIR) -g \
          -I$(KERNEL_INCLUDE_DIR) -Wall -nostdinc -nostdlib

LDFLAGS= -lOSSO-special -L$(LIB_DIR) 


OBJECTS=LOADER STARTER installer BOOTLOADER INIT


all: $(OBJECTS)


BOOTLOADER: bootloader.nasm
	$(AS) bootloader.nasm -DFAT12 -f bin -o BOOTLOADER

STARTER: kernel-setup.nasm
	$(AS) kernel-setup.nasm -f bin -o STARTER 

LOADER: kernel-loader.c parser.c parser.h
	$(BCC) -Ms -ansi kernel-loader.c -o LOADER

installer: installer.c
	@$(CC) installer.c -o installer
	@strip installer

INIT: init.o parser.o
	ld $(LIB_DIR)/startup_special.o init.o parser.o -o INIT $(LDFLAGS)#-lOSSO -L$(LIB_DIR) $(LDFLAGS)

init.o: init.c parser.h
	$(CC) $(INITFLAGS) -c init.c -o init.o

parser.o: parser.c parser.h
	$(CC) -Wall -c parser.c -o parser.o


clean:
	rm -f $(OBJECTS) *.o Makefile.Dep Makefile.Dep.bak



#########################################################################
#                              Disk config                              #
#########################################################################


#TARGET        = o:/
#TARGET_IMG    = /tmp/osso.disk

#TARGET        = x:/
#TARGET_IMG    = /dev/hdc

#TARGET_FLOPPY        = a:/
#TARGET_FLOPPY_IMG    = /dev/fd0



#include ../drivers/Makefile.config

#T_BOOT_DIR    = BOOT
#T_MODULES_DIR = MODULES

T_BOOT      = $(TARGET)$(T_BOOT_DIR)
T_MODULES   = $(TARGET)$(T_MODULES_DIR)

T_BOOT_FLOPPY      = $(TARGET_FLOPPY)$(T_BOOT_DIR)
T_MODULES_FLOPPY   = $(TARGET_FLOPPY)$(T_MODULES_DIR)

disk: all
	./installer $(TARGET_IMG)
	mcopy -o LOADER $(TARGET)
	mcopy -o INITCONF $(TARGET)
	-mmd -o $(T_BOOT)
	-mmd -o $(T_MODULES)
	mcopy -o STARTER $(T_BOOT)
	mcopy -o INIT $(T_BOOT)
#	mcopy -o ../kernel/kernel.bin $(T_BOOT)/KERNEL
#	mcopy -o ../lib/libOSSO.so $(T_BOOT)
#	mcopy -o ../drivers/mps $(T_BOOT)
#	mcopy -o ../test/SHELL $(T_MODULES)
#	cd ../drivers && mcopy -o $(DRIVERS) $(T_MODULES)


floppy: all
	./installer $(TARGET_FLOPPY_IMG)
	mcopy -o LOADER $(TARGET_FLOPPY)
	mcopy -o INITCONF $(TARGET_FLOPPY)
	-mmd -o $(T_BOOT_FLOPPY)
	-mmd -o $(T_MODULES_FLOPPY)
	mcopy -o STARTER $(T_BOOT_FLOPPY)
	mcopy -o INIT $(T_BOOT_FLOPPY)
#	mcopy -o ../kernel/kernel.bin $(T_BOOT_FLOPPY)/KERNEL
#	mcopy -o ../lib/libOSSO.so $(T_BOOT_FLOPPY)
#	mcopy -o ../drivers/mps $(T_BOOT_FLOPPY)
#	mcopy -o ../test/SHELL $(T_MODULES_FLOPPY)
#	cd ../drivers && mcopy -o $(DRIVERS) $(T_MODULES_FLOPPY)


#########################################################################
#                              Dep Target                               #
#########################################################################

Makefile.Dep: dep

SOURCES = $(wildcard *.c) $(wildcard *.s)

dep:
	@echo ------------------- Dependeces in [boot] ----------------------
#	@echo sources=$(SOURCES)
#	@echo include=$(INCLUDEDIR)
	@echo "">Makefile.Dep
	@makedepend -I$(KERNEL_INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) -I$(DRIVER_INCLUDE_DIR) $(SOURCES) -f Makefile.Dep


EXIST-DEP=$(wildcard Makefile.dep)
ifeq ($EXIST-DEP,Makefile.dep)
-include Makefile.Dep
endif
