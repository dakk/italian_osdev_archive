# Rules shared between multiple Makefiles #

%.o: %.S
	@echo AS  $<
	$(AS) $(CFLAGS) -c -o $@ $<

%.o: %.c
	@echo CC  $<
	$(CC) $(CFLAGS) -c -o $@ $<

ifeq ($(TOPDIR)/.depend,$(wildcard $(TOPDIR)/.depend))
include $(TOPDIR)/.depend
endif
