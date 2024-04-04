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


######################### Driver's LIB #OSSO# ################################
# File: drivers/lib/Makefile                                                 #
# Makefile lib for drivers                                                   #
##############################################################################
# $Id: Makefile,v 1.11 2001/08/23 19:21:11 iurlano Exp $
##############################################################################
# $Log: Makefile,v $
# Revision 1.11  2001/08/23 19:21:11  iurlano
# Changed IDE to publish correctly each partition, whole disk included.
# Added mount point support to vfs. Extended its protocol.
# Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
# Now it is possible to mount a partition and actually see it int the FS!!
# Works!
# Changed fs.c in libOSSO to use the vfs for dir and file access.
# Changed mount utility to inspect partitions and mount points state.
# Added Documentation for FAT driver
#
# Revision 1.10  2001/08/15 20:06:36  iurlano
# Added DMA management functions into the kernel and library
#
# Revision 1.9  2001/04/10 23:10:26  iurlano
# Little changes to the Makefiles
# Check the inline asm instructions
# check whether the kernel compiles with -O (strange behaviour, as expected)
# Removed some warnings
#
# Revision 1.8  2001/04/08 16:32:58  iurlano
# Added dep target to each Makefile
# Now dependencies are automatically remade after a make clean
#
# Revision 1.7  2001/03/31 15:07:50  iurlano
# bugfixes
#
# Revision 1.6  2001/03/25 15:27:25  steros
# Divided MPS interfaces in more structured files.
#
# Revision 1.5  2001/03/10 17:08:48  steros
# Added header and lib functions to interface driver to MPS.
#
# Revision 1.4  2000/11/24 18:25:54  iurlano
# Added first try of a DMA memory management. Yet to be tested
#
# Revision 1.3  2000/08/25 13:39:37  iurlano
# Added CHANGELOG
# more translations
#
# Revision 1.2  2000/08/04 17:47:58  iurlano
# Translation in progress
#
# Revision 1.1.1.1  2000/05/30 08:11:02  iurlano
# OSSO : First Public Release
#
##############################################################################

CFLAGS = -I$(LIB_INCLUDE_DIR) -I$(KERNEL_INCLUDE_DIR) -Wall #--pedantic


#OBJECTS = io.o pci.o dma_memory.o drivers.o drv2mps.o 

OBJECTS = drivers.o io.o pci.o dma_memory.o dma.o # mpsint.o drv2mps.o prg2mps.o 


lib: libOSSODRV.a( $(OBJECTS) )

clean:
	rm *.o *~ include/*~ libOSSODRV.a Makefile.Dep Makefile.Dep.bak -f

Makefile.Dep: dep

SOURCES = $(wildcard *.c) $(wildcard *.s)
dep:
	@echo ----------------- Dependeces in [drivers/lib] -------------------
#	@echo sources=$(SOURCES)
#	@echo include=$(INCLUDEDIR)
	@echo "">Makefile.Dep
	@makedepend -I$(KERNEL_INCLUDE_DIR) -I$(LIB_INCLUDE_DIR) $(SOURCES) -f Makefile.Dep


EXIST-DEP=$(wildcard Makefile.dep)
ifeq ($EXIST-DEP,Makefile.dep)
-include Makefile.Dep
endif