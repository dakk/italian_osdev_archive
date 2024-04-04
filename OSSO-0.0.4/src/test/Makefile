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


############################### OSSO ## TEST #################################
# File: test/Makefile                                                        #
# Makefile directory test                                                    #
##############################################################################
# $Id: Makefile,v 1.22 2001/08/09 10:37:53 iurlano Exp $
##############################################################################
# $Log: Makefile,v $
# Revision 1.22  2001/08/09 10:37:53  iurlano
# Change Creat_thread to accept an integer parameter to be passed to the thread.
# corrected exit_thread behaviour.
# Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
#
# Revision 1.21  2001/07/30 17:07:29  steros
# MPS multiple copies drivers implemented.
#
# Revision 1.20  2001/07/01 15:17:32  iurlano
# Added first prototype of partition management system
# Added mount program
#
# Revision 1.19  2001/06/26 21:00:41  iurlano
# Added mem information program.
# Changed shell mem command to meminfo
#
# Revision 1.18  2001/06/24 22:18:12  iurlano
# Optimized process creation functions
# Added programs to the test section in xconfig
# Added some functions to kernel-loader.c
# Page faults now terminate the running process
# Bugfixed memory leak in program execution from the shell
#
# Revision 1.17  2001/06/10 17:17:47  steros
# Working on fat basic functions.
#
# Revision 1.16  2001/05/29 22:36:18  iurlano
# Worked on xconfig tools. Now loads and saves and has new instructions
# Changed keyboard driver to work with events subsystem
#
# Revision 1.15  2001/05/01 01:04:19  iurlano
# Work on Dynamic Linker problem
# Seems like that the shared MPS_Pid is working now.
# I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
# for the other case, I've to work on it and I don't think I'm going to do it very soon.
#
# Revision 1.14  2001/04/26 22:09:09  iurlano
# fat work (again)
# working snapshot ?
#
# Revision 1.13  2001/04/26 19:25:54  iurlano
# fat work
# bugfixes
#
# Revision 1.12  2001/04/20 20:45:36  steros
# Added GetDriverPid function for driver pid retrival from MPS.
#
# Revision 1.11  2001/04/18 21:39:43  iurlano
# Fixed a very insidious memory allocation bug in (getbmsg())
# little change in xconfig
# fixed stability problems
# added two programs for testing message passing
# Added freemsg()
# no memory leak now on message passing
# the shell seems to work!!
# A strange problem: bochs reports freepage page already freed where the real pc and vmware do not!
#
# Revision 1.10  2001/04/17 18:04:12  iurlano
# Fixed compilation code
# Warning: This is a very unstable snapshot
# There are a lot of strange problems
#
# Revision 1.9  2001/04/15 14:15:43  iurlano
# Fix in drivers/Makefile
# Added two syscall for mps pid (experimental)
# Added first attempt of a shell
# changed and multithreaded keyboard driver
# changed startup code
# added libOSSO-special for special processes like mps & INIT
# added get_char() in libOSSO
#
# Revision 1.8  2001/04/11 15:26:32  iurlano
# Thread and processes termination work
# Memory leak
#
# Revision 1.7  2001/04/10 23:10:26  iurlano
# Little changes to the Makefiles
# Check the inline asm instructions
# check whether the kernel compiles with -O (strange behaviour, as expected)
# Removed some warnings
#
# Revision 1.6  2001/04/09 15:10:12  marcopex
# Eventest3.c finnally works
#
# Revision 1.5  2001/04/08 16:32:58  iurlano
# Added dep target to each Makefile
# Now dependencies are automatically remade after a make clean
#
# Revision 1.4  2001/04/06 10:27:02  marcopex
# Continue event testing...
#
# Revision 1.3  2001/04/03 16:50:05  marcopex
# Some little event testing & debugging
#
# Revision 1.2  2000/08/22 14:57:55  iurlano
# More Translation and cleaning
#
# Revision 1.1.1.1  2000/05/30 08:11:21  iurlano
# OSSO : First Public Release
#
##############################################################################


CFLAGS=-g -I$(LIB_INCLUDE_DIR) -I$(DRIVER_INCLUDE_DIR) -Wall -nostdinc -O2 -fexceptions

LDFLAGS=-g -lOSSO -o TEST4 -L$(LIB_DIR) $(LIB_DIR)/startup.o


# FORKTEST JULIA provacom.com
TEST_PROGRAMS= EVENTES3 FORKTEST FORKTST2 SHELL MPSCOM TESTMSG TESTMSG2 MPSTEST TEST HIWORLD THREADT

include Makefile.config

#it is NOT an error. TESTPROGRAMS and TEST_PROGRAMS are different!
all: $(TESTPROGRAMS) $(LIB_DIR)/libOSSO.so

provacom.com: test.o
	ld test.o -lOSSO -o provacom.com -L$(LIB_DIR) $(LIB_DIR)/startup.o

clean:
	rm -f *.o *~ TEST3 $(TEST_PROGRAMS) a.out provacom.com Makefile.Dep Makefile.Dep.bak

HIWORLD: hiworld.o
	ld hiworld.o -lOSSO -o HIWORLD -L$(LIB_DIR) $(LIB_DIR)/startup.o

TEST3: test3.o
	ld test3.o -lOSSO -o TEST3 -L$(LIB_DIR) $(LIB_DIR)/startup.o

TEST: test.o
	ld test.o -lOSSO -o TEST -L$(LIB_DIR) $(LIB_DIR)/startup.o

TESTPID: testpid.o
	ld testpid.o -lOSSO -o TESTPID -L$(LIB_DIR) $(LIB_DIR)/startup.o

TEST4: test4.o
	ld test4.o -lOSSO -o TEST4 -L$(LIB_DIR) $(LIB_DIR)/startup.o

MEM: mem.o
	ld mem.o -lOSSO -o MEM -L$(LIB_DIR) $(LIB_DIR)/startup.o

mount: mount.o
	ld mount.o -lOSSO -o mount -L$(LIB_DIR) $(LIB_DIR)/startup.o



TESTMSG: message-test.o
	 ld message-test.o -o TESTMSG -L$(LIB_DIR) $(LIB_DIR)/startup.o -lOSSO

TESTMSG2: message-test-2.o
	 ld message-test-2.o -o TESTMSG2 -L$(LIB_DIR) $(LIB_DIR)/startup.o -lOSSO

CONDIV: condivision.o
	 ld condivision.o -o CONDIV -L$(LIB_DIR) $(LIB_DIR)/startup.o -lOSSO

THREADT: threadtest.o
	 ld threadtest.o -o THREADT -L$(LIB_DIR) $(LIB_DIR)/startup.o -lOSSO

FORKTEST: forktest.o
	 ld forktest.o -o FORKTEST -L$(LIB_DIR) $(LIB_DIR)/startup.o -lOSSO

FORKTST2: forktest2.o
	 ld forktest2.o -o FORKTST2 -L$(LIB_DIR) $(LIB_DIR)/startup.o -lOSSO

SHELL: shell.o
	 ld shell.o -o SHELL -L$(LIB_DIR) -L$(DRIVER_LIB_DIR) $(LIB_DIR)/startup.o -lOSSO -lOSSODRV

MPSCOM: mpscom.o
	 ld mpscom.o -o MPSCOM -L$(LIB_DIR) -L$(DRIVER_LIB_DIR) $(LIB_DIR)/startup.o -lOSSO -lOSSODRV

MPSTEST: mpstest.o
	 ld mpstest.o -o MPSTEST -L$(LIB_DIR) -L$(DRIVER_LIB_DIR) $(LIB_DIR)/startup.o -lOSSO -lOSSODRV

DRIVERTEST: driver-test.o
	 ld driver-test.o -lOSSO -o DRIVER -L$(LIB_DIR) $(LIB_DIR)/startup.o

CHDTEST: chdtest.o
	ld chdtest.o -lOSSO -o CHDTEST -L$(LIB_DIR) $(LIB_DIR)/startup.o

#JULIA: julia.o
#	ld julia.o -Bstatic -lOSSO -o JULIA  -L$(LIB_DIR) $(LIB_DIR)/startup.o -lm

EVENTEST: eventest.o
	ld eventest.o -lOSSO -o EVENTEST -L$(LIB_DIR) $(LIB_DIR)/startup.o

EVENTES2: eventest2.o
	ld eventest2.o -lOSSO -o EVENTES2  -L$(LIB_DIR) $(LIB_DIR)/startup.o

EVENTES3: eventest3.o
	ld eventest3.o -lOSSO -o EVENTES3 -L$(LIB_DIR) $(LIB_DIR)/startup.o

T_BOOT      = $(TARGET)$(T_BOOT_DIR)
T_MODULES   = $(TARGET)$(T_MODULES_DIR)

T_BOOT_FLOPPY      = $(TARGET_FLOPPY)$(T_BOOT_DIR)
T_MODULES_FLOPPY   = $(TARGET_FLOPPY)$(T_MODULES_DIR)

disk: all
	mcopy -o $(TESTPROGRAMS) $(T_MODULES)

floppy: all
	mcopy -o $(TESTPROGRAMS) $(T_MODULES_FLOPPY)

Makefile.Dep: dep

SOURCES = $(wildcard *.c) $(wildcard *.s)

dep:
	@echo -n Calculating new Dependences...
#	@echo sources=$(SOURCES)
#	@echo include=$(INCLUDEDIR)
	@echo "">Makefile.Dep
	@makedepend -I$(KERNEL_INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) \
		-I$(DRIVER_INCLUDE_DIR) $(SOURCES) -f Makefile.Dep
	@echo " Done"

EXIST-DEP=$(wildcard Makefile.dep)
ifeq ($EXIST-DEP,Makefile.dep)
-include Makefile.Dep
endif
# DO NOT DELETE
