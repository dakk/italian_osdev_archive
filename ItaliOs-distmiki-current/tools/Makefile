INSTALL_DIR = /usr/local/bin/italios
LIB_DIR = /usr/local/share/italios
INCLUDE_DIR = /usr/local/include/italios

VERSIONE=0.2

CDIR=$(basename $(PWD))

all:
	gcc func.c -o it_func 
install:
	-mkdir $(INSTALL_DIR)
	-mkdir /usr/local/share
	-mkdir $(LIB_DIR)
	-mkdir $(INCLUDE_DIR)
	cp it_func $(INSTALL_DIR)
	cp libc.d $(LIB_DIR)
	cp start.d $(LIB_DIR)
	cp makefile.template $(LIB_DIR)
	cp it_prj_gen $(INSTALL_DIR)
	cp -R include/* $(INCLUDE_DIR)
	cp link.ld $(LIB_DIR)
clean:
	rm it_func

dist:
	make clean
	tar czvf ../ItaliOs-Tools-$(VERSIONE).tar.gz $(CDIR)
	cd $(CDIR)
