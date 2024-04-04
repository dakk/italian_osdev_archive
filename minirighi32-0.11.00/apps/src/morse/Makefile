#       Makefile for Minirighi applications
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OBJS		= morse.o
TARGET		= $(APPSBIN)/morse

all: $(OBJS)
	$(LD) -o$(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

include $(APPS)/Rules.make
