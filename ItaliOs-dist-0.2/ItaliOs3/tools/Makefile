INSTALL_DIR = /usr/local/bin/italios
LIB_DIR = /usr/local/share/italios
INCLUDE_DIR = /usr/local/include/italios

VERSIONE=0.3

CDIR=$(basename $(PWD))

install:
	-mkdir $(INSTALL_DIR)
	-mkdir /usr/local/share
	-mkdir $(LIB_DIR)
	-mkdir $(INCLUDE_DIR)
	cp start.d $(LIB_DIR)
	cp makefile.template $(LIB_DIR)
	cp it_prj_gen $(INSTALL_DIR)
	cp -R include/* $(INCLUDE_DIR)
	cp link.ld $(LIB_DIR)

dist:
	make clean
	tar czvf ../ItaliOs-Tools-$(VERSIONE).tar.gz $(CDIR)
	cd $(CDIR)
