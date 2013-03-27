VPATH = src
CFLAGS = -Os -pipe -fpack-struct -Wall -Wno-unused -fno-caller-saves -fno-aggressive-loop-optimizations
LD= $(CC)
OS_LAYER = osl-linux.c

all: lld2d lld2test

include src/common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend

romfs:
	$(ROMFSINST) -S init.d/W19lld2d /etc/rc.d/W19lld2d
	$(ROMFSINST) -s /etc/rc.d/W19lld2d /etc/init.d/lld2d
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/lld2d
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/icon.ico
	$(ROMFSINST) -S -e CONFIG_USER_LLTD	/bin/icon.large.ico
	$(ROMFSINST) -e CONFIG_USER_LLTD lld2d.conf /etc/lld2d.conf

-include .depend
