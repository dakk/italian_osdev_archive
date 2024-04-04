#compilatore

LIBNAME2=libnet2.a
LIBNAME=libnet.a
VER=\"$(shell date "+%d%m%y")\"

# Programma per cancellare i file oggetto e gli eseguibili

# Gli oggetti da compilare per creare il kernel
OBJS2 = modem.o seriale.o funzcomuni.o ppp.o locale.o ip.o tcp.o icmp.o udp.o socket.o
OBJS = main.o telnet.o dns.o irc.o

all : $(OBJS) $(OBJS2)
	$(AR) $(ARFLAGS)  $(LIBNAME2) $(OBJS2) > /dev/null
	$(AR) $(ARFLAGS)  $(LIBNAME) $(OBJS) > /dev/null

# Compilo gli altri
.c.o:
	$(CC)  $(CFLAG) -DVER=$(VER) -c -o $@ $<


# cancella
clean:
	$(REMOVE) $(OBJS) $(LIBNAME)
	$(REMOVE) $(OBJS2) $(LIBNAME2)

indent:
	$(INDENT) *.c
	rm *~

rete:
	cd .. && make rete

install:
	cd .. && make install

