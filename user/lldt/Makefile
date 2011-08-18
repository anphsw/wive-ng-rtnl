VPATH = src
CFLAGS += -Os -pipe -Wall -Wno-unused -s -fpack-struct -Wpadded
LD= $(CC)
OS_LAYER = osl-linux.c

all: lld2d lld2test

include src/common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend

romfs:
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/lld2d
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/icon.ico
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/icon.large.ico
	$(ROMFSINST)	lld2d.conf		/etc/lld2d.conf

-include .depend
