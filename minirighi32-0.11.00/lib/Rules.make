# Rules shared between library Makefiles #

AR	= ar rcs
CFLAGS	= -Wall -pipe -O2 -fomit-frame-pointer -nostdinc -fno-builtin -I$(INCLUDE)

%.o: %.c
	@echo CC  $<
	$(CC) $(CFLAGS) -c -o $@ $<

