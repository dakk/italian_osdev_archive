# Rules shared between multiple applications' Makefiles #

CFLAGS		= -Wall -O2 -fomit-frame-pointer -nostdinc -nostdlib -fno-builtin -I$(INCLUDE)

LDFLAGS		= -N -S -X -nostdlib -lc -L$(LIB) -T$(LDSCRIPT)
LDFLAGS16 	= -Ttext 0x10000 -s --oformat=binary
LDSCRIPT 	= $(APPS)/app.ld

%.o: %.S
	@echo AS  $<
	$(AS) $(CFLAGS) -c -o $@ $<

%.o16: %.S
	@echo AS  $<
	$(AS) $(CFLAGS) -c -o$@ $<

%.o: %.c
	@echo CC  $<
	$(CC) $(CFLAGS) -c -o $@ $<

