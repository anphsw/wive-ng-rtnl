NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/drivers/char
LDFLAGS += -lnvram

EXEC =
ifdef CONFIG_RT2880_RADAEMON
EXEC += nvram_daemon
endif
ifdef CONFIG_RT2880_RAINIT
EXEC += ralink_init
endif

all: $(EXEC)

ifdef CONFIG_RT2880_RADAEMON
nvram_daemon: nvram_daemon.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
endif

ifdef CONFIG_RT2880_RAINIT
ralink_init: ralink_init.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
endif

clean:
	rm -f *.o $(EXEC)

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	ln -sf ralink_init nvram_get
	ln -sf ralink_init nvram_set
	mv -f nvram_get $(ROMFSDIR)/bin
	mv -f nvram_set $(ROMFSDIR)/bin
