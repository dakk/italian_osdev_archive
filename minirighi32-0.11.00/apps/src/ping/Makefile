#       Makefile for Minirighi applications
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OBJS		= ping.o
TARGET		= $(APPSBIN)/ping

all: $(OBJS)
	$(LD) -o$(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

include $(APPS)/Rules.make
