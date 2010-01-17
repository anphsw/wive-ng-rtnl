EXEC = networkmap
OBJS = function.o networkmap.o

SHDIR = $(ROOTDIR)/user/shared
USERDIR = $(ROOTDIR)/user

CFLAGS  += -Wall -DASUS -DASUS_NVRAM -DBCMNVRAM -I. -I$(SHDIR)/include -I$(LINUXINCDIR) 
CFLAGS  += -s -Os -DNO_PARALLEL
LDFLAGS += -L$(SHDIR) -L$(USERDIR)/asus/netconf -lshared -lnetconf


all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)


romfs:
	$(ROMFSINST) /usr/sbin/$(EXEC)

clean: 
	rm -rf *.o $(EXEC) *~
