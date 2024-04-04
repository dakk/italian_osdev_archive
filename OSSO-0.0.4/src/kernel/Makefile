#
# OSSO - A Micro Kernel OS
# Copyright (c) 2000 Alessandro Iurlano.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#


####################################OSSO######################################
# File: kernel/Makefile                                                      #
# Makefile for kernel directory                                              #
##############################################################################

##############################################################################
# $Id: Makefile,v 1.22 2001/08/15 20:06:36 iurlano Exp $
##############################################################################
#
# $Log: Makefile,v $
# Revision 1.22  2001/08/15 20:06:36  iurlano
# Added DMA management functions into the kernel and library
#
# Revision 1.21  2001/05/29 22:36:18  iurlano
# Worked on xconfig tools. Now loads and saves and has new instructions
# Changed keyboard driver to work with events subsystem
#
# Revision 1.20  2001/05/14 21:33:13  iurlano
# First work on smp systems (thanks to smpbochs)
#
# Revision 1.19  2001/04/15 14:15:43  iurlano
# Fix in drivers/Makefile
# Added two syscall for mps pid (experimental)
# Added first attempt of a shell
# changed and multithreaded keyboard driver
# changed startup code
# added libOSSO-special for special processes like mps & INIT
# added get_char() in libOSSO
#
# Revision 1.18  2001/04/10 23:10:26  iurlano
# Little changes to the Makefiles
# Check the inline asm instructions
# check whether the kernel compiles with -O (strange behaviour, as expected)
# Removed some warnings
#
# Revision 1.17  2001/04/09 14:10:53  iurlano
# Added BOCHS feature
# Corrected a bug iin process.h
#
# Revision 1.16  2001/04/08 16:32:58  iurlano
# Added dep target to each Makefile
# Now dependencies are automatically remade after a make clean
#
# Revision 1.15  2001/03/06 11:50:26  iurlano
# Added functions for smp multitasking (not yet tested)
#
# Revision 1.14  2001/03/02 17:56:13  iurlano
# changed an ld flag in the kernel Makefile. COULD BE A PROBLEM!
# some reordering
#
# Revision 1.13  2000/12/12 22:28:59  iurlano
# Threadified fork(). To be tested but seems to be working
# Added support for coloured printk() in kernel.
#
# Revision 1.12  2000/12/07 14:09:43  iurlano
# Debug in progress
#
# Revision 1.11  2000/12/07 12:50:50  marcopex
# *** empty log message ***
#
# Revision 1.10  2000/12/06 21:08:50  iurlano
# Thread work in progress
# Include files work
# Cut some warnings
#
# Revision 1.9  2000/12/05 20:59:47  iurlano
# Thread work in progress. Yet to make first complete subsystem
#
# Revision 1.8  2000/12/05 16:36:01  iurlano
# Thread managementwork in progress
#
# Revision 1.7  2000/11/27 16:45:55  iurlano
# Changes on Makefiles
# Work on bcc loading
#
# Revision 1.6  2000/11/24 18:25:54  iurlano
# Added first try of a DMA memory management. Yet to be tested
#
# Revision 1.5  2000/11/09 14:30:31  iurlano
# Added pci.c module for accessing pci with configuration mechanism 1
#    without BIOS
#
# Revision 1.4  2000/11/02 23:34:13  iurlano
# Dynamic linking seems to work!!
# Some problems at boot stage with files with extension
#
# Revision 1.3  2000/08/22 14:57:55  iurlano
# More Translation and cleaning
#
# Revision 1.2  2000/07/27 12:13:07  iurlano
# Translation in progress
#
# Revision 1.1.1.1  2000/05/30 08:11:08  iurlano
# OSSO : First Public Release
#
##############################################################################



CFLAGS = -Wall -I$(KERNEL_INCLUDE_DIR) -nostdinc -g -ffreestanding# -O#--pedantic

# Object file to be put into the kernel

# Warning: kernel.o must be the first
OBJECTS = kernel.o memset.o printk.o interrupts.o io.o sysinfo.o \
          string.o sys.o syscall_handler.o \
          syscall.o gdt.o interrupt_handlers.o   \
          process.o timer.o scheduler.o paging.o memory.o PnP.o semaphore.o \
          panic.o signals.o messages.o chardevs.o devices.o pci.o \
          dma_memory.o events.o coda_pronti.o lock.o global_data.o smp.o \
	  dma.o

# Removed # hd.o parttable.o fat.o keyboard.o 

all: kernel.bin

kernel.bin: $(OBJECTS)
#	@echo Linking Kernel ....
	ld $(OBJECTS) -o kernel.bin --oformat binary -Ttext 0x20000 # -Map MapFile
#	@echo Linking Done.

kernel.elf: $(OBJECTS)
	ld $(OBJECTS) -o kernel.elf -Ttext 0x20000

clean:
	rm *.o *~ kernel.bin kernel.elf a.out MapFile Makefile.Dep Makefile.Dep.bak -f

T_BOOT      = $(TARGET)$(T_BOOT_DIR)
T_MODULES   = $(TARGET)$(T_MODULES_DIR)

T_BOOT_FLOPPY      = $(TARGET_FLOPPY)$(T_BOOT_DIR)
T_MODULES_FLOPPY   = $(TARGET_FLOPPY)$(T_MODULES_DIR)

disk: all
	mcopy -o kernel.bin $(T_BOOT)/KERNEL

floppy: all
	mcopy -o kernel.bin $(T_BOOT_FLOPPY)/KERNEL



Makefile.Dep: dep

SOURCES = $(wildcard *.c) $(wildcard *.s)

dep:
	@echo ------------------- Dependeces in [kernel] ---------------------
#	@echo sources=$(SOURCES)
#	@echo include=$(INCLUDEDIR)
	@echo "">Makefile.Dep
	@makedepend -I$(KERNEL_INCLUDE_DIR) $(SOURCES) -f Makefile.Dep

EXIST-DEP=$(wildcard Makefile.dep)
ifeq ($EXIST-DEP,Makefile.dep)
-include Makefile.Dep
endif