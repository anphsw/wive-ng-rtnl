#NVDIR = $(ROOTDIR)/lib/libnvram #replaced by ASUS nvram
SHDIR = $(ROOTDIR)/user/shared
#CFLAGS += -I$(NVDIR)
CFLAGS += -DASUS_NVRAM -I$(SHDIR)/include -I$(ROOTDIR)/linux-2.6.21.x/include
LDFLAGS += -L$(SHDIR) -lshared
EXEC = ated
SRCS = ated.c

#EXTRA_CFLAGS += -DDBG

all: $(EXEC)

$(EXEC): $(SRCS) $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.o

