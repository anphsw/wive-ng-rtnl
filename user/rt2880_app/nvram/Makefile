NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char
LDFLAGS += -lnvram
EXEC = nvram_daemon ralink_init

all: $(EXEC)

nvram_daemon: nvram_daemon.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ralink_init: ralink_init.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o $(EXEC)

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	ln -sf ralink_init nvram_get
	ln -sf ralink_init nvram_set
	mv -f nvram_get $(ROMFSDIR)/bin
	mv -f nvram_set $(ROMFSDIR)/bin

