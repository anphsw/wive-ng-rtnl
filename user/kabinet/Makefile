#stupid ISP (as kabinet) auth daemon

LDFLAGS += -s
LIBS = md5.o rmd160.o
lanauth: lanauth.o $(LIBS)
	$(CC) $(CCFLAGS) $(LDFLAGS) lanauth.o -o lanauth $(LIBS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o lanauth

install:
	echo "Ok.."

romfs:
	cp -fv lanauth $(ROMFSDIR)/bin/lanauth
