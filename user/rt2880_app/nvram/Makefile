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
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_get
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_buf_get
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_set
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_show
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_default
