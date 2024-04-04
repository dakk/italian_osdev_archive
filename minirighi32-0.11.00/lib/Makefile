#       Makefile for Minirighi library
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OBJS	= \
	  $(LIB)/ctype/ctype.o \
	  \
	  $(LIB)/errno/errno.o \
	  \
	  $(LIB)/stdio/getline.o \
	  $(LIB)/stdio/gets.o \
	  $(LIB)/stdio/printf.o \
	  $(LIB)/stdio/puts.o \
	  $(LIB)/stdio/scanf.o \
	  $(LIB)/stdio/vsprintf.o \
	  \
	  $(LIB)/stdlib/atoi.o \
	  $(LIB)/stdlib/calloc.o \
	  $(LIB)/stdlib/getopt.o \
	  $(LIB)/stdlib/rand.o \
	  \
	  $(LIB)/string/string.o \
	  \
	  $(LIB)/time/asctime.o \
	  $(LIB)/time/ctime.o \
	  $(LIB)/time/localtime.o \
	  \
	  $(LIB)/unistd/sleep.o

TARGET	= $(LIB)/libc.a

all: $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS)

include $(LIB)/Rules.make
