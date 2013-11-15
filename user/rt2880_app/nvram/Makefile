NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) -I$(ROOTDIR)/$(LINUXDIR)/include
LDFLAGS += -lnvram

EXEC += ralink_init

all: $(EXEC)

ralink_init: ralink_init.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o ralink_init

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_get
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_buf_get
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_set
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_show
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_default
	$(ROMFSINST) -s /bin/ralink_init /bin/nvram_clear
