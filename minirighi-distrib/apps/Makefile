#       Makefile for Minirighi applications
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

SUBDIRS		= src/basename \
		  src/beep \
		  src/date \
		  src/echo \
		  src/fork \
		  src/hello \
		  src/morse \
		  src/ping \
		  src/rubik \
		  src/scrsvr \
		  src/sleep \
		  src/socket \
		  src/threads \
		  src/tictactoe \
		  src/usleep \
		  src/v86test

all:
	@for dir in $(SUBDIRS); \
	do( $(MAKE) -C $$dir all ); \
	done;

clean:
	@for dir in $(SUBDIRS); \
	do( $(MAKE) -C $$dir clean ); \
	done;
