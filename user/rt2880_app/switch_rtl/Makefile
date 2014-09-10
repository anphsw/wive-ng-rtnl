EXEC = rtl8367m

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR) -I$(ROOTDIR)/$(LINUXDIR)/include
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h

SRC=rtl8367m.c

all: $(EXEC)
switch: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
