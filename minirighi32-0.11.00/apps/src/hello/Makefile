#       Makefile for Minirighi applications
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OBJS		= hello.o
TARGET		= $(APPSBIN)/hello

all: $(OBJS)
	$(LD) -o$(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

include $(APPS)/Rules.make
