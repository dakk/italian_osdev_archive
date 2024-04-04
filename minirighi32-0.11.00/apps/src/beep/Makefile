#       Makefile for Minirighi applications
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OBJS		= beep.o
TARGET		= $(APPSBIN)/beep

all: $(OBJS)
	$(LD) -o$(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

include $(APPS)/Rules.make
