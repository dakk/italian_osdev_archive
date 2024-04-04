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
# File: Makefile                                                             #
# Main Makefile                                                              #
##############################################################################
# $Id: Makefile,v 1.33 2001/05/30 10:56:32 iurlano Exp $
##############################################################################
# $Log: Makefile,v $
# Revision 1.33  2001/05/30 10:56:32  iurlano
# bugfixes
#
# Revision 1.32  2001/05/29 22:36:18  iurlano
# Worked on xconfig tools. Now loads and saves and has new instructions
# Changed keyboard driver to work with events subsystem
#
# Revision 1.31  2001/04/28 12:15:52  steros
# OOPS: corrected stupid commit description!!!
#
# Revision 1.30  2001/04/28 11:30:40  steros
# Rearranging tree: committing new directory include files.
#
# Revision 1.29  2001/04/23 14:43:05  iurlano
# work on FAT module
# work on FS protocol
#
# Revision 1.28  2001/04/15 14:15:42  iurlano
# Fix in drivers/Makefile
# Added two syscall for mps pid (experimental)
# Added first attempt of a shell
# changed and multithreaded keyboard driver
# changed startup code
# added libOSSO-special for special processes like mps & INIT
# added get_char() in libOSSO
#
# Revision 1.27  2001/04/08 16:32:58  iurlano
# Added dep target to each Makefile
# Now dependencies are automatically remade after a make clean
#
# Revision 1.26  2001/04/06 09:52:17  iurlano
# *** empty log message ***
#
# Revision 1.25  2001/04/05 21:06:13  iurlano
# Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
#
# Revision 1.24  2001/04/05 14:10:53  iurlano
# xconfig changes
#
# Revision 1.23  2001/04/04 11:11:08  iurlano
# *** empty log message ***
#
# Revision 1.22  2001/04/03 21:34:44  iurlano
# Dynamic semaphores work in progress
# Little changes in make xconfig
# Added and changed in the code the value of THREAD_INVALID from 0 to -1
#
# Revision 1.21  2001/03/25 15:27:25  steros
# Divided MPS interfaces in more structured files.
#
# Revision 1.20  2001/03/16 19:22:59  iurlano
# Added something to the make xconfig prog
# Now OSSO runs on plex86
#
# BIG PROBLEM. OSSO gives random errors about finding INIT and relocating
#   and doesn't start
#
# Revision 1.19  2001/03/14 15:17:36  iurlano
# Corrected CVS changes
#
# Revision 1.18  2001/03/12 15:09:54  marcopex
# Finished (maybe) the compilation of Events
#
# Revision 1.17  2001/03/07 21:18:03  iurlano
# Trying another way for colorgcc
# Added a new type in xconfig parameter file: define
#
# Revision 1.16  2001/03/06 22:52:10  iurlano
# Added lock mechanism to P and V. To be tested
# Added miniman xconfig Make target
#
# Revision 1.15  2001/03/06 11:50:25  iurlano
# Added functions for smp multitasking (not yet tested)
#
# Revision 1.14  2001/03/05 23:02:40  iurlano
# Some corrections
# Little modifies to Makefile
#
# Revision 1.13  2001/01/08 09:52:54  steros
# New boot procedure implemented.
#
# Revision 1.12  2000/12/05 16:36:01  iurlano
# Thread managementwork in progress
#
# Revision 1.11  2000/11/30 22:47:13  iurlano
# Changed gdt handling.
# Removed LDT for each task and added a global LDT for USER LEVEL tasks
#
# Revision 1.10  2000/11/28 15:14:50  iurlano
# Minor changes
#
# Revision 1.9  2000/11/27 16:45:53  iurlano
# Changes on Makefiles
# Work on bcc loading
#
# Revision 1.8  2000/11/27 13:36:25  iurlano
# Created Doc directory
#
# Revision 1.7  2000/11/23 11:23:20  steros
# Added code for bcc compiled pre-kernel.
#
# Revision 1.6  2000/11/03 18:00:29  iurlano
# *** empty log message ***
#
# Revision 1.5  2000/11/02 23:34:12  iurlano
# Dynamic linking seems to work!!
# Some problems at boot stage with files with extension
#
# Revision 1.4  2000/09/25 09:01:51  steros
# Modificato per funzionare anche su immagini.
#
# Revision 1.3  2000/08/22 14:57:54  iurlano
# More Translation and cleaning
#
# Revision 1.2  2000/06/18 11:55:33  iurlano
# Corrected some comments. Added doc in README
#
# Revision 1.1.1.1  2000/05/30 08:10:55  iurlano
# OSSO : First Public Release
#
##############################################################################


AS=nasm
#SHELL=/bin/csh

CC = gcc

#SHELL=/bin/sh

ROOT := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

KERNEL_SOURCES = $(wildcard kernel/*.c) $(wildcard kernel/*.s)
KERNE_INCLUDES = $(wildcard kernel/include/*.h)

export CC
export KERNEL_DIR=$(ROOT)/kernel
export KERNEL_INCLUDE_DIR=$(ROOT)/kernel/include
export LIB_DIR=$(ROOT)/lib
export LIB_INCLUDE_DIR=$(ROOT)/include
export DRIVER_DIR=$(ROOT)/drivers
export DRIVER_LIB_DIR=$(DRIVER_DIR)/lib
export DRIVER_INCLUDE_DIR=$(ROOT)/include/drivers

#Use this to color output (Nice!)
#export CC=colorgcc


#CFLAGS= -Wall --pedantic
CFLAGS= -Wall

CONFIGURATION_FILES = Makefile.config Config_Global.h Config_Kernel.h  Config_Boot.h drivers/Makefile.config test/Makefile.config


-include Makefile.config

all: Kernel Lib Boot
#all2: boot/BOOTLOADER boot/PRE-KERNEL kernel/kernel.bin

xconfig:
	wish config.tcl

disk: all Drivers Test
	@echo "                **********     Making boot disk     **********"
	@cd boot; make disk
	@cd kernel; make disk
	@cd lib; make disk
	@cd drivers; make disk
	@cd test; make disk
	@echo "                **********      Boot disk done      **********"

floppy: all Drivers
	@echo "                **********     Making boot floppy    **********"
	@cd boot; make floppy
	@cd kernel; make floppy
	@cd lib; make floppy
	@cd drivers; make floppy
	@cd test; make floppy
	@echo "                **********      Boot floppy done    **********"

#Disk: all
#	@echo "Creating FAT boot disk ...."
#	@mcd /
#	@mcopy -o boot/PRE-KERNEL a:LOADER
#	@mcopy -o kernel/kernel.bin a:KERNEL
#	@cd boot && ./installer /dev/fd0
#
#Disk-new: all
#	@echo "Creating new generation FAT boot disk ...."
#	@mcd /
#	@mcopy -o boot/LOADER a:LOADER
#	@mcopy -o boot/STARTER a:STARTER
#	@mcopy -o kernel/kernel.bin a:KERNEL
#	@cd boot && ./installer /dev/fd0

Boot: Kernel Lib
	@echo "                **********  Compiling BOOT    part  **********"
	@cd boot && make
	@echo "                **********  Finished  BOOT    part  **********"

Kernel:
	@echo "                **********  Compiling KERNEL  part  **********"
	@cd kernel && make
	@echo "                **********  Finished  KERNEL  part  **********"

clean:
	(cd boot;make clean)
	(cd kernel;make clean)
	(cd test;make clean)
	(cd lib;make clean)
	(cd drivers;make clean)
	(cd drivers/lib;make clean)
	-rm doc/*~ -f
#	-rm $(CONFIGURATION_FILES)

Lib:
	@echo "                **********  Compiling LIBRARY part  **********"
	cd lib;make
	cd drivers/lib;make
	@echo "                **********  Finished  LIBRARY part  **********"

Test:
	cd test;make

Drivers:
	@echo "                **********  Compiling DRIVERS part  **********"
	cd drivers;make
	@echo "                **********  Finished  DRIVERS part  **********"
# UTILITIES

dep:
	@cd kernel && make dep
	@cd boot && make dep
	@cd lib && make dep
	@cd drivers && make dep
	@cd test && make dep

archive: 
	make dep && make clean && make all && make clean
	cd .. ; tar zcvf OSSO-`date +%d-%m-%Y`.tar.gz src ; \
	cp OSSO-`date +%d-%m-%Y`.tar.gz Archive/ ; \
	cp OSSO-`date +%d-%m-%Y`.tar.gz ARCHIVE-BACKUP/

# DO NOT DELETE
