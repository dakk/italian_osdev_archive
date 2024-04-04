#       Toplevel makefile for Minirighi
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

.EXPORT_ALL_VARIABLES:
export MAKE

# Kernel name #
KERNELNAME	= minirighi32 (ia-32)

# Kenrel version #
MAINVERSION	= 0
SUBVERSION	= 11
SUBLEVEL	= 00
VERSION		= $(MAINVERSION).$(SUBVERSION).$(SUBLEVEL)

# Directories #
TOPDIR		= $(shell /bin/pwd)

APPS		= $(TOPDIR)/apps
APPSBIN		= $(TOPDIR)/apps/bin
BIN		= $(TOPDIR)/bin
DOC		= $(TOPDIR)/doc
INCLUDE		= $(TOPDIR)/include
LIB		= $(TOPDIR)/lib
KERNEL		= $(TOPDIR)/os
SCRIPTS		= $(TOPDIR)/scripts

# Files #
CONFIG		= $(TOPDIR)/.config
CONFIGH		= $(INCLUDE)/kernel/config.h
TARGET		= $(TOPDIR)/bin/mr32.elf
ASMDUMP		= $(TOPDIR)/bin/mr32.dis
GZTARGET	= $(TOPDIR)/bin/mr32.gz
MAP		= $(TOPDIR)/bin/mr32.map
MTOOLSRC	= $(TOPDIR)/.mtoolsrc
FDIMAGE		= $(BIN)/minirighi-floppy.img
CDIMAGE		= $(BIN)/minirighi-cdrom.iso

ifeq ($(TOPDIR)/.config,$(wildcard $(TOPDIR)/.config))
include $(TOPDIR)/.config
endif

# Options #
LDFLAGS		= -g -nostdlib -S -X -L$(LIB) -lc
CFLAGS		= -Wall -pipe \
		  -I$(INCLUDE) \
		  -O3 -finline-functions \
		  -fomit-frame-pointer -mpreferred-stack-boundary=2 \
		  -nostdinc -fno-builtin

ifdef CONFIG_M386
CFLAGS += -march=i386
endif

ifdef CONFIG_M486
CFLAGS += -march=i486
endif

ifdef CONFIG_M586
CFLAGS += -march=i586
endif

ifdef CONFIG_M586TSC
CFLAGS += -march=i586 -mcpu=pentium
endif

ifdef CONFIG_M586MMX
CFLAGS += -march=i586 -mcpu=pentium-mmx
endif

ifdef CONFIG_M686
CFLAGS += -march=i686 -mcpu=pentiumpro
endif

ifdef CONFIG_MPENTIUMII
CFLAGS += -march=i686 -mcpu=pentium2
endif

ifdef CONFIG_MPENTIUMIII
CFLAGS += -march=i686 -mcpu=pentium3
endif

ifdef CONFIG_MPENTIUM4
CFLAGS += -march=i686 -mcpu=pentium4
endif

ifdef CONFIG_MK6
CFLAGS += $(shell if $(CC) -march=k6 -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-march=k6"; else echo "-march=i586"; fi)
endif

ifdef CONFIG_MK7
CFLAGS += $(shell if $(CC) -march=athlon -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-march=athlon"; else echo "-march=i686 -malign-functions=4"; fi)
endif

ifdef CONFIG_MATHLONXP
CFLAGS += $(shell if $(CC) -march=athlon-xp -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-march=athlon"; else echo "-march=i686 -malign-functions=4"; fi)
endif

# Programs #
MAKE		= make -s
AS		= gcc
CC		= gcc
LD		= ld
OBJDUMP		= objdump -D -l -x -t
NASM		= nasm -f elf
GRUB		= /usr/sbin/grub

#########################################################################
#                                                                       #
#  Rules for kernel, library & applications.                            #
#                                                                       #
#########################################################################

all: kernel
	@echo -e "Well done."
	@echo -e "Don't forget to run \"make install\" to copy the kernel on the floppy image."
	@echo

world:
	@echo -e "\n--- Configuration ---\n"
	$(MAKE) config
	@echo -e "\n--- Make dependencies ---\n"
	$(MAKE) dep
	@echo -e "\n--- Cleaning ---\n"
	$(MAKE) clean
	@echo -e "\n--- Making the kernel ---\n"
	$(MAKE) all
	@echo -e "\n--- Installing the kernel to floppy image ---\n"
	$(MAKE) install
	@echo -e "\n--- Making applications ---\n"
	$(MAKE) apps
	@echo -e "\n--- Installing applications to floppy image ---\n"
	$(MAKE) apps_install
	@echo -e "\nWorld done... enjoy!\n"

kernel: .config .depend lib
	@echo Making kernel...
	@$(MAKE) -C $(KERNEL)

install: .config .depend kernel $(FDIMAGE)
	@echo "Copying kernel to floppy image $(FDIMAGE)"
	@mcopy -n $(GZTARGET) a:/kernel/
	@sync
	@echo "done."

lib:
	@echo Making library...
	@$(MAKE) -C $(LIB)

apps: .config lib $(FDIMAGE)
	@echo Making applications...
	@$(MAKE) -C $(APPS) all
	@echo "done."

apps_install:
	@echo -n "Copying applications to floppy image $(FDIMAGE)... "
	@mcopy -n $(APPSBIN)/* a:/bin/
	@sync
	@echo "done."

apps_clean:
	@$(MAKE) -C $(APPS) clean

clean: apps_clean
	@$(MAKE) -C $(KERNEL) clean
	@$(MAKE) -C $(LIB) clean
	@find . -name *~ -print | xargs rm -f

config:
	@$(SCRIPTS)/config.sh
	@echo -e "Configuration done."
	@echo -e "Now it's time to run \"make dep\" to find all dependencies.\n"

depend dep: .config
	@rm -f .depend
	@echo -n "Finding dependencies for all C files... "
	@$(foreach i, $(shell find ${TOPDIR} -name \*.c -print), \
		echo -n $(dir $i) >> .depend; $(CC) -I$(INCLUDE) -M $i >> .depend;)
	@echo "done."
	@echo -e "Now it's time to run \"make\" to compile the kernel.\n"

.depend:
	@echo -e "\nYou have to run \"make dep\" before!!!\n"
	@exit 1

.config:
	@echo -e "\nYou have to run \"make config\" before!!!\n"
	@exit 1

#########################################################################
#                                                                       #
#  Rules for the installation device images.                            #
#                                                                       #
#########################################################################

$(FDIMAGE):
	@$(SCRIPTS)/mkfdimg.sh

isocd: install $(FDIMAGE)
	@mkdir cd_root
	@cp $(FDIMAGE) cd_root/`basename $(FDIMAGE)`
	@mkisofs -o $(CDIMAGE) -b `basename $(FDIMAGE)` -R -J cd_root
	@rm -rf cd_root
	@echo -e "\nISO bootable cd image created $(CDIMAGE)\n"

disk: install
	dd bs=8192 if=$(FDIMAGE) of=/dev/fd0

#########################################################################
#                                                                       #
#  Rules for backup, documentation, etc...                              #
#                                                                       #
#########################################################################

distclean: clean
	@$(MAKE) -C $(KERNEL) distclean
	@$(MAKE) -C $(DOC) clean
	@rm -f $(CDIMAGE) $(FDIMAGE) $(MTOOLSRC) $(CONFIG) $(CONFIGH) .depend tags
	@echo Clean done.

backup: distclean
	cd .. && tar cf - `basename $(TOPDIR)`/ | bzip2 -c -9 > minirighi32-$(VERSION).tar.bz2
	sync
	@echo -e "Backup done.\n"

documentation doc:
	@$(MAKE) -C $(DOC) all
	@echo
	@echo -e "Documentation done in $(DOC).\n"

tags:
	find . -name '*.[chS]' | xargs ctags -a

wc:
	find . -type f -name *.[Sch] -print | sort | xargs wc -l

.PHONY: config depend dep kernel all lib install apps disk isocd clean \
	distclean backup doc documentation wc world

include Rules.make
