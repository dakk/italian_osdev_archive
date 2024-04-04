# Makefile per il modulo task del kernel core di Exabyte

PATH_TO_TOP = ..
OBJS = todo.o
EXEC = task.o

include $(PATH_TO_TOP)/make.rules

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(EXEC)
