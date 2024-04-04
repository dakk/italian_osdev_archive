#       Makefile for Minirighi applications
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OBJS		= v86test.o16
TARGET		= $(APPSBIN)/v86test.bin

all: $(OBJS)
	$(LD) -o$(TARGET) $(OBJS) $(LDFLAGS16)

clean:
	rm -f $(OBJS) $(TARGET)

include $(APPS)/Rules.make
