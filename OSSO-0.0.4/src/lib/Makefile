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


############################### LIB #OSSO# ###################################
# File: lib/Makefile                                                         #
# Makefile directory lib                                                     #
##############################################################################
# $Id: Makefile,v 1.21 2001/05/29 22:36:18 iurlano Exp $
##############################################################################
# $Log: Makefile,v $
# Revision 1.21  2001/05/29 22:36:18  iurlano
# Worked on xconfig tools. Now loads and saves and has new instructions
# Changed keyboard driver to work with events subsystem
#
# Revision 1.20  2001/05/01 01:04:19  iurlano
# Work on Dynamic Linker problem
# Seems like that the shared MPS_Pid is working now.
# I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
# for the other case, I've to work on it and I don't think I'm going to do it very soon.
#
# Revision 1.19  2001/04/22 20:04:29  iurlano
# added fs management module in libOSSO
# work on fs protocol
#
# Revision 1.18  2001/04/22 13:17:38  iurlano
# Changed mpstest
# mps now working
# integrated some code for pid asking in the libosso
#
# Revision 1.17  2001/04/17 18:04:12  iurlano
# Fixed compilation code
# Warning: This is a very unstable snapshot
# There are a lot of strange problems
#
# Revision 1.16  2001/04/15 14:15:43  iurlano
# Fix in drivers/Makefile
# Added two syscall for mps pid (experimental)
# Added first attempt of a shell
# changed and multithreaded keyboard driver
# changed startup code
# added libOSSO-special for special processes like mps & INIT
# added get_char() in libOSSO
#
# Revision 1.15  2001/04/10 23:10:26  iurlano
# Little changes to the Makefiles
# Check the inline asm instructions
# check whether the kernel compiles with -O (strange behaviour, as expected)
# Removed some warnings
#
# Revision 1.14  2001/04/08 16:32:58  iurlano
# Added dep target to each Makefile
# Now dependencies are automatically remade after a make clean
#
# Revision 1.13  2001/04/06 10:27:02  marcopex
# Continue event testing...
#
# Revision 1.12  2001/04/05 14:10:54  iurlano
# xconfig changes
#
# Revision 1.11  2001/04/03 21:34:44  iurlano
# Dynamic semaphores work in progress
# Little changes in make xconfig
# Added and changed in the code the value of THREAD_INVALID from 0 to -1
#
# Revision 1.10  2001/04/03 16:50:05  marcopex
# Some little event testing & debugging
#
# Revision 1.9  2001/03/31 15:07:50  iurlano
# bugfixes
#
# Revision 1.8  2001/03/05 23:02:40  iurlano
# Some corrections
# Little modifies to Makefile
#
# Revision 1.7  2000/11/30 22:47:15  iurlano
# Changed gdt handling.
# Removed LDT for each task and added a global LDT for USER LEVEL tasks
#
# Revision 1.6  2000/11/02 23:35:38  iurlano
# Dynamic linking seems to work!!
# Some problems at boot stage with files with extension
#
# Revision 1.5  2000/11/02 11:59:37  iurlano
# ELF dynamic linking work in progress
#
# Revision 1.4  2000/11/01 23:23:52  iurlano
# Work on dynamic linking
#
# Revision 1.3  2000/10/29 21:09:02  iurlano
# Working on ELF and Dynamic Loader
#
# Revision 1.2  2000/10/23 23:33:44  iurlano
# Remove ebx references from the libOSSO
#
# Revision 1.1.1.1  2000/05/30 08:11:18  iurlano
# OSSO : First Public Release
#
# Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
#
#
# Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
#
#
# Revision 1.9  2000/01/16 22:43:32  iurlano
# Aggiunta tabella dei drivers e relative funzioni.
# da testare
#
# Revision 1.8  2000/01/06 17:00:03  iurlano
# Aggiunto albero directory 'drivers'
# Aggiunte alcune syscalls
#
# Revision 1.7  1999/12/21 13:59:19  iurlano
# Cazzatine
#
# Revision 1.6  1999/12/21 13:46:54  pecciant
# Aggiunte READ/WRITE e dispositivi a caratteri
#
# Revision 1.5  1999/12/17 22:46:06  iurlano
#
# I messaggi sembrano funzionare !!!!
#
# Revision 1.4  1999/12/17 17:26:58  iurlano
# Aggiunte alcune syscall, tipo getpid
# Lavoro su sistema di messaggi
# Migliorata libreria
#
# Revision 1.3  1999/12/02 15:02:43  iurlano
#
# Aggiustato files nella directory lib
#
#
##############################################################################

CFLAGS=-I$(LIB_INCLUDE_DIR) -I$(KERNEL_INCLUDE_DIR) -I$(KERNEL_DIR)  -shared -fpic -Wall# -g

OBJECTS = ld.o ctype.o printc.o memset.o string.o signals.o messages.o \
          processes.o memory.o unistd.o chardevs.o semaphores.o events.o \
          tty.o ../drivers/lib/prg2mps.o ../drivers/lib/mpsint.o \
	  ../drivers/lib/drv2mps.o fs.o

all:lib


lib: $(OBJECTS) startup.o libOSSO.so libOSSO-special.a
#lib: libOSSO.a( $(OBJECTS) ) startup.o  #libOSSO.so

libOSSO-special.a: libOSSO-special.a($(OBJECTS)) startup_special.o
#	$(CC) -e _dt_Linker -o libOSSO-special.so -shared -fPIC $(OBJECTS) -nostdinc -nostdlib  startup_special.o

libOSSO.so: $(OBJECTS) #startup.o
	$(CC) -e _dt_Linker -o libOSSO.so -shared -fPIC $(OBJECTS) -nostdinc -nostdlib # startup.o

#ld.so: ld.c $(OBJECTS)
#	gcc $(CFLAGS) -c -o ld.so -shared -fpic ld.c -nostdinc -nostdlib

# Miscellaneous target
clean:
	rm *.o *~ include/*~ libOSSO.a libOSSO-special.a libOSSO-special.so libOSSO.so Makefile.Dep Makefile.Dep.bak -f

T_BOOT      = $(TARGET)$(T_BOOT_DIR)
T_MODULES   = $(TARGET)$(T_MODULES_DIR)

T_BOOT_FLOPPY      = $(TARGET_FLOPPY)$(T_BOOT_DIR)
T_MODULES_FLOPPY   = $(TARGET_FLOPPY)$(T_MODULES_DIR)

disk: all
	mcopy -o libOSSO.so $(T_BOOT)

floppy: all
	mcopy -o libOSSO.so $(T_BOOT_FLOPPY)



Makefile.Dep: dep


SOURCES = $(wildcard *.c) $(wildcard *.s)

dep:
	@echo ------------------- Dependeces in [lib] ---------------------
#	@echo sources=$(SOURCES)
#	@echo include=$(INCLUDEDIR)
	@echo "">Makefile.Dep
	@makedepend -I$(KERNEL_INCLUDE_DIR) -I$(KERNEL_DIR) -I$(LIB_INCLUDE_DIR) $(SOURCES) -f Makefile.Dep


EXIST-DEP=$(wildcard Makefile.dep)
ifeq ($EXIST-DEP,Makefile.dep)
-include Makefile.Dep
endif