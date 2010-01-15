
BIN = sigtest
INSTALLDIR = $(ROOTDIR)/romfs

all: ${BIN}
	${STRIP} ${BIN}

crc_gen: sigtest.c
	gcc -o sigtest sigtest.c

romfs:
	$(ROMFSINST) /usr/sbin/$(BIN)

clean:
	-rm -f *.o sigtest a.out
