#       Makefile for Minirighi documentation
#
#       Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>

OUTPUT_DIR	= $(DOC)/alldocs
DOXYFILE	= Doxyfile.conf

export DOXYFILE

all: clean
	@$(SCRIPTS)/mkdoxycfg.sh
	doxygen $(DOXYFILE)
clean:
	rm -rf $(OUTPUT_DIR)
	rm -f $(DOXYFILE)

.PHONY: all clean
	
