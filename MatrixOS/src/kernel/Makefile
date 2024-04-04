
OBJECTS= boot/boot.o \
			main.o \
			boot/mboot.o \
			libc/stdlib.o \
			libc/string.o \
			task/scheduler.o \
			kframework/stringutil.o \
			kservices/kshell.o \
			kservices/kout.o \
			kservices/kdbg.o \
			kservices/klog.o \
			kservices/kerr.o \
			hardware/cpu/gdt.o \
			hardware/cpu/idt.o \
			hardware/cpu/inthandler.o \
			hardware/cpu/irq.o \
			hardware/cpu/cpuid.o \
			hardware/driver/keyboard.o \
			hardware/driver/video.o \
			hardware/driver/dma.o \
			hardware/driver/rtc.o \
			hardware/driver/vbeconsole.o \
			hardware/driver/vbe.o \
			hardware/driver/floppy.o \
			hardware/driver/keytraslation.o \
			hardware/driver/bochsdebug.o \
			hardware/driver/pit.o \
			hardware/driver/initconsole.o \
			libc++/string.o \
			memory/virtualmem.o \
			memory/paging.o \
			memory/physicalmem.o \
			syscall.o

HEADERS= header/boot/mboot.h \
			header/boot/multiboot.h \
			header/libc/stdlib.h \
			header/libc/string.h \
			header/task/scheduler.h \
			header/kframework/stringutil.h \
			header/kframework/kframework.h \
			header/kservices/kdbg.h \
			header/kservices/kerr.h \
			header/kservices/klog.h \
			header/kservices/kout.h \
			header/kservices/kshell.h \
			header/syscall.h \
			header/hardware/cpu/cpuid.h \
			header/hardware/cpu/bitops.h \
			header/hardware/cpu/gdt.h \
			header/hardware/cpu/idt.h \
			header/hardware/cpu/irq.h \
			header/hardware/cpu/inthandler.h \
			header/hardware/io.h \
			header/hardware/driver/bochsdebug.h \
			header/hardware/driver/vbefont.h \
			header/hardware/driver/initconsole.h \
			header/hardware/driver/vbematrixlogo.h \
			header/hardware/driver/dma.h \
			header/hardware/driver/pit.h \
			header/hardware/driver/rtc.h \
			header/hardware/driver/vbe.h \
			header/hardware/driver/keytraslation.h \
			header/hardware/driver/video.h \
			header/hardware/driver/floppy.h \
			header/hardware/driver/keyboard.h \
			header/hardware/driver/vbeconsole.h \
			header/kdef.h \
			header/memory/paging.h \
			header/memory/physicalmem.h \
			header/memory/virtualmem.h

CC=gcc
CXX=g++
AS=as

INCLUDES=-I./header/ \
			-I./header/libc \
			-I./header/libc++

KERNEL_NAME=matrixos

CFLAGS=-nostdlib -fomit-frame-pointer -Wall -march=i386 -fno-builtin -fno-exceptions -fno-rtti -c $(INCLUDES)
CXXFLAGS=$(CFLAGS)
ASFLAGS=-c

LDFLAGS=-o../../bin/$(KERNEL_NAME) -Ttext 0x100000 -Map kernel.map

all: kernel

kernel: $(OBJECTS)
	@ echo -n "Linko il kernel... "
	@ ld $(LDFLAGS) $(OBJECTS) 2>&1 >> build_output
	@ echo "OK"

.cpp.o: $(HEADERS)
	@ echo -n "CXX $@ ... "
	@ if [ `$(CXX) -v 2>&1 | tail -n 1 | cut -d ' ' -f 3 | cut -d . -f 1` -eq 4 ]; then $(CXX) $(CXXFLAGS) -fno-threadsafe-statics -o $@ $<; else  $(CXX) $(CXXFLAGS) -o $@ $<; fi 2>&1 >> build_output
	@ echo "OK"

.S.o: $(HEADERS)
	@ echo "AS $@"
	@ $(CC) $(INCLUDES) -c -o $@ $< 2>&1 >> build_output

clean:
	@ echo -n "Rimuovo tutti i file oggetto e il kernel..."
	@ rm -f $(OBJECTS) ../../bin/$(KERNEL_NAME)
	@ echo -n > build_output
	@ echo "Fatto"
