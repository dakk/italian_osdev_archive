#       Makefile for Minirighi kernel
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

# Files #
LDSCRIPT	= $(KERNEL)/arch/i386/kernel.ld
OBJS		= \
		$(KERNEL)/arch/i386/boot/start.o \
		\
		$(KERNEL)/arch/i386/kernel/clock.o \
		$(KERNEL)/arch/i386/kernel/cpu.o \
		$(KERNEL)/arch/i386/kernel/delay.o \
		$(KERNEL)/arch/i386/kernel/doublefault.o \
		$(KERNEL)/arch/i386/kernel/exc.o \
		$(KERNEL)/arch/i386/kernel/exception.o \
		$(KERNEL)/arch/i386/kernel/exit.o \
		$(KERNEL)/arch/i386/kernel/irq.o \
		$(KERNEL)/arch/i386/kernel/interrupt.o \
		$(KERNEL)/arch/i386/kernel/mem.o \
		$(KERNEL)/arch/i386/kernel/paging.o \
		$(KERNEL)/arch/i386/kernel/panic.o \
		$(KERNEL)/arch/i386/kernel/syscall.o \
		$(KERNEL)/arch/i386/kernel/v86.o \
		\
		$(KERNEL)/kernel/console.o \
		$(KERNEL)/kernel/cow.o \
		$(KERNEL)/kernel/debug.o \
		$(KERNEL)/kernel/elf32.o \
		$(KERNEL)/kernel/klogd.o \
		$(KERNEL)/kernel/kmalloc.o \
		$(KERNEL)/kernel/kpagerd.o \
		$(KERNEL)/kernel/main.o \
		$(KERNEL)/kernel/sched.o \
		$(KERNEL)/kernel/semaphore.o \
		$(KERNEL)/kernel/syscall.o \
		$(KERNEL)/kernel/task.o \
		\
		$(KERNEL)/mm/umalloc.o \
		\
		$(KERNEL)/drivers/ata/Ide.o\
		$(KERNEL)/drivers/ata/IdeTimer.o\
		$(KERNEL)/drivers/ata/IdeDebug.o\
		$(KERNEL)/drivers/ata/IdeLow.o\
		$(KERNEL)/drivers/ata/IdeAta.o\
		$(KERNEL)/drivers/dma/dma.o \
		$(KERNEL)/drivers/floppy/floppy.o \
		$(KERNEL)/drivers/keyboard/keyboard.o \
		$(KERNEL)/drivers/rs232/serial.o \
		$(KERNEL)/drivers/video/video.o \
		$(KERNEL)/drivers/pci/pci.o \
		$(KERNEL)/drivers/rtl8139/rtl8139.o \
		$(KERNEL)/drivers/speaker/speaker.o \
		$(KERNEL)/drivers/time/time.o \
		\
		$(KERNEL)/fs/fat/fat12.o \
		$(KERNEL)/fs/ext2/ext2.o \
		$(KERNEL)/fs/vfs/vfs.o \
		\
		$(KERNEL)/network/arp.o \
		$(KERNEL)/network/eth.o \
		$(KERNEL)/network/ip.o \
		$(KERNEL)/network/icmp.o \
		$(KERNEL)/network/socket.o \
		$(KERNEL)/network/tcp.o \
		$(KERNEL)/network/udp.o \
		\
		$(KERNEL)/shell/minishell.o

# Rules #

all: kernel

kernel: $(ASMDUMP) $(GZTARGET)

$(TARGET): $(OBJS) $(LDSCRIPT)
	$(LD) -T$(LDSCRIPT) -Map $(MAP) -o $@ $(OBJS) $(LDFLAGS)

$(ASMDUMP): $(TARGET)
	$(OBJDUMP) $< > $@

$(GZTARGET): $(TARGET)
	strip -s -o$< $<
	gzip -c -9 $(TARGET) > $(GZTARGET)

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(GZTARGET) $(TARGET) $(ASMDUMP) $(MAP)

# Include #

include $(TOPDIR)/Rules.make
