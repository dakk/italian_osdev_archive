#       Makefile for Minirighi applications
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OBJS		= socket.o
TARGET		= $(APPSBIN)/socket

all: $(OBJS)
	$(LD) -o$(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

include $(APPS)/Rules.make
