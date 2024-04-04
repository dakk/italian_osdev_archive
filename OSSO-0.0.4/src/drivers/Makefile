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


################################## OSSO ######################################
# File: drivers/Makefile                                                     #
# Makefile drivers directory                                                 #
##############################################################################
# $Id: Makefile,v 1.33 2001/08/23 19:21:11 iurlano Exp $
##############################################################################
# $Log: Makefile,v $
# Revision 1.33  2001/08/23 19:21:11  iurlano
# Changed IDE to publish correctly each partition, whole disk included.
# Added mount point support to vfs. Extended its protocol.
# Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
# Now it is possible to mount a partition and actually see it int the FS!!
# Works!
# Changed fs.c in libOSSO to use the vfs for dir and file access.
# Changed mount utility to inspect partitions and mount points state.
# Added Documentation for FAT driver
#
# Revision 1.32  2001/08/15 20:06:36  iurlano
# Added DMA management functions into the kernel and library
#
# Revision 1.31  2001/08/13 23:55:27  iurlano
# FDC Work in progress, but needs DMA
# DMA work started.
# Some warnings fixed in kernel code
#
# Revision 1.30  2001/07/01 15:17:32  iurlano
# Added first prototype of partition management system
# Added mount program
#
# Revision 1.29  2001/06/24 17:06:36  steros
# Added vfs.
#
# Revision 1.28  2001/06/10 22:46:37  iurlano
# Added hiworld program to xconfig panel
#
# Revision 1.27  2001/05/29 22:36:18  iurlano
# Worked on xconfig tools. Now loads and saves and has new instructions
# Changed keyboard driver to work with events subsystem
#
# Revision 1.26  2001/05/29 17:41:17  steros
# Modified printc for line buffered functionality.
#
# Revision 1.25  2001/05/27 20:22:53  iurlano
# Added experimental driver config feature
#
# Revision 1.24  2001/05/26 14:56:14  steros
# Added serial logging facility.
#
# Revision 1.23  2001/05/01 13:47:39  steros
# Improved MPS.
#
# Revision 1.22  2001/04/19 15:17:58  iurlano
# Fixed some bug
# changed ide driver
# changed fat driver (cabled the pid of the ide driver)
# changed shell( now accept 3 commands mem, info, ls) TRY IT
# VERY STABLE SNAPSHOT
#
# Revision 1.21  2001/04/17 18:04:12  iurlano
# Fixed compilation code
# Warning: This is a very unstable snapshot
# There are a lot of strange problems
#
# Revision 1.20  2001/04/15 14:15:43  iurlano
# Fix in drivers/Makefile
# Added two syscall for mps pid (experimental)
# Added first attempt of a shell
# changed and multithreaded keyboard driver
# changed startup code
# added libOSSO-special for special processes like mps & INIT
# added get_char() in libOSSO
#
# Revision 1.19  2001/04/10 23:10:26  iurlano
# Little changes to the Makefiles
# Check the inline asm instructions
# check whether the kernel compiles with -O (strange behaviour, as expected)
# Removed some warnings
#
# Revision 1.18  2001/04/08 16:32:58  iurlano
# Added dep target to each Makefile
# Now dependencies are automatically remade after a make clean
#
# Revision 1.17  2001/03/31 15:07:50  iurlano
# bugfixes
#
# Revision 1.16  2001/03/10 17:08:48  steros
# Added header and lib functions to interface driver to MPS.
#
# Revision 1.15  2001/02/07 21:28:40  iurlano
# added vga driver prototype
#
# Revision 1.14  2001/01/08 09:52:54  steros
# New boot procedure implemented.
#
# Revision 1.13  2000/12/08 16:45:27  iurlano
# Thread Work in progress.
# First working version reached. Needs Debug.
#
# Revision 1.12  2000/11/30 22:47:15  iurlano
# Changed gdt handling.
# Removed LDT for each task and added a global LDT for USER LEVEL tasks
#
# Revision 1.11  2000/11/27 13:36:25  iurlano
# Created Doc directory
#
# Revision 1.10  2000/11/26 23:34:59  iurlano
# work in progress on DMA memory
#
# Revision 1.9  2000/11/24 18:25:54  iurlano
# Added first try of a DMA memory management. Yet to be tested
#
# Revision 1.8  2000/11/24 11:28:32  iurlano
# Added floppy driver prototype
#
# Revision 1.7  2000/11/02 11:59:37  iurlano
# ELF dynamic linking work in progress
#
# Revision 1.6  2000/10/23 23:33:44  iurlano
# Remove ebx references from the libOSSO
#
# Revision 1.5  2000/10/23 15:21:24  iurlano
# Working on ELF support
#
# Revision 1.4  2000/10/16 18:41:54  iurlano
# Added prototype of rtl8139 network card driver
#
# Revision 1.3  2000/08/04 17:47:58  iurlano
# Translation in progress
#
# Revision 1.2  2000/06/18 11:55:34  iurlano
# Corrected some comments. Added doc in README
#
# Revision 1.1.1.1  2000/05/30 08:10:59  iurlano
# OSSO : First Public Release
#
##############################################################################


CFLAGS=-g -I$(LIB_INCLUDE_DIR) -I$(DRIVER_INCLUDE_DIR) \
       -Wall -nostdinc -nostdlib

LDFLAGS= -g -lOSSODRV -lOSSO -L$(LIB_DIR) -Llib $(LIB_DIR)/startup.o
LDFLAGS_MPS= -lOSSODRV -lOSSO-special -L$(LIB_DIR) -Llib $(LIB_DIR)/startup_special.o

#OBJECTS= mps KEYBOARD FAT VGA  IDE SERIAL # RTL8139 THREAD # TEST-DMA FDC
#Experimental Makefile configuration

DRIVERS = 

include Makefile.config

OBJECTS = mps vfs $(DRIVERS)

all: $(OBJECTS) #\
     #$(LIB_DIR)/libOSSO.a lib/libOSSODRV.a
	strip $(OBJECTS)
Linux:
	cd linux;make

mps: mps.o mps_struct.o mps_boot.o mps_error.o
	ld mps.o mps_struct.o mps_boot.o mps_error.o -o mps $(LDFLAGS_MPS)

vfs: vfs.o
	ld vfs.o -o vfs $(LDFLAGS)

TEST-DMA: test-dma.o
	ld test-dma.o  -o TEST-DMA $(LDFLAGS)

THREAD: test-threads.o
	ld test-threads.o  -o THREAD $(LDFLAGS)
clean:
	rm *.o *~ $(OBJECTS) Makefile.Dep Makefile.Dep.bak -f

KEYBOARD: keyboard.o keyscancodes.h
	ld keyboard.o  -o KEYBOARD $(LDFLAGS)

SERIAL: serial.o
	ld serial.o -o SERIAL $(LDFLAGS)

IDE: ide.o
	ld ide.o -o IDE $(LDFLAGS)

FDC: fdc.o
	ld fdc.o -o FDC $(LDFLAGS)

FAT: fat.o
	ld fat.o -o FAT $(LDFLAGS)
VGA: vga.o
	ld vga.o -o VGA $(LDFLAGS)

RTL8139: rtl8139.o
	ld rtl8139.o -o RTL8139 $(LDFLAGS)

RTL8139-2: rtl8139-2.o rtl8139-2.h
	ld rtl8139-2.o -o RTL8139-2 $(LDFLAGS)

#FDC: fdc.o util.o
#	ld fdc.o util.o -o FDC $(LDFLAGS)
LINUX-RTL8139: linux-rtl8139.o pci-scan.o
	ld linux-rtl8139.o -o LINUX-RTL8139 pci-scan.o $(LDFLAGS)

T_BOOT      = $(TARGET)$(T_BOOT_DIR)
T_MODULES   = $(TARGET)$(T_MODULES_DIR)

T_BOOT_FLOPPY      = $(TARGET_FLOPPY)$(T_BOOT_DIR)
T_MODULES_FLOPPY   = $(TARGET_FLOPPY)$(T_MODULES_DIR)

disk: all
	mcopy -o mps vfs $(T_BOOT)
	mcopy -o $(DRIVERS) $(T_MODULES)

floppy: all
	mcopy -o mps vfs $(T_BOOT_FLOPPY)
	mcopy -o $(DRIVERS) $(T_MODULES_FLOPPY)


Makefile.Dep: dep

SOURCES = $(wildcard *.c) $(wildcard *.s)

dep:
	@cd lib && make dep
	@echo ------------------- Dependeces in [drivers] ---------------------
#	@echo sources=$(SOURCES)
#	@echo include=$(INCLUDEDIR)
	@echo "">Makefile.Dep
	@makedepend -I$(KERNEL_INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) -I$(DRIVER_INCLUDE_DIR) -I$(KERNEL_INCLUDE_DIR)  $(SOURCES) -f Makefile.Dep


EXIST-DEP=$(wildcard Makefile.dep)
ifeq ($EXIST-DEP,Makefile.dep)
-include Makefile.Dep
endif
