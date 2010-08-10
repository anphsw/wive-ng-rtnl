# Jacky.Yang 29-Nov-2007
# Makefile for sysd
#
# Makefile by Jacky Yang,
# 
# Get Board ID from EEPROM
#

EXEC = getBoardID
NVDIR = $(ROOTDIR)/lib/libnvram
CFLAGS += -I$(NVDIR) 
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char
LDFLAGS += -lnvram

all: $(EXEC)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) /usr/sbin/$(EXEC)

install:
	$(ROMFSINST) /usr/sbin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o