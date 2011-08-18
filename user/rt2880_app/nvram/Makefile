NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/include
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
	rm -f *.o nvram_daemon ralink_init

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) -S /bin/$$i ; done
	ln -sf ralink_init nvram_get
	ln -sf ralink_init nvram_set
	ln -sf ralink_init nvram_show
	ln -sf ralink_init nvram_default
	mv -f nvram_get $(ROMFSDIR)/bin
	mv -f nvram_set $(ROMFSDIR)/bin
	mv -f nvram_show $(ROMFSDIR)/bin
	mv -f nvram_default $(ROMFSDIR)/bin
