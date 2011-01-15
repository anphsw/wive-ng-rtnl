VPATH = src
CFLAGS += -Os -pipe -Wall -Wno-unused -s -fpack-struct -Wpadded  -D__LITTLE_ENDIAN
LD= $(CC)
OS_LAYER = osl-linux.c

all: lld2d lld2test

include src/common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend

romfs:
	$(ROMFSINST) -e CONFIG_USER_LLTD       /bin/lld2d
	$(ROMFSINST) -e CONFIG_USER_LLTD       /bin/icon.ico
	$(ROMFSINST) -e CONFIG_USER_LLTD       /bin/icon.large.ico

-include .depend
