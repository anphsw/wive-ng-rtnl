#
# Linux router shared code Makefile
#
# Copyright 2005, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.1.1.1 2007/01/25 12:52:22 jiahao_jhou Exp $
#

CFLAGS	+= -I. -I./include -I../httpd -Wall -DASUS_NVRAM -D__CONFIG_NAT__ -DDLM -DCDMA -I../../linux-2.6.21.x/include

CFLAGS	+= -s
LDFLAGS += -L.

# 2008.10 magic. add notify_rc.o
OBJS := shutils.o crc.o http.o defaults.o nvram_linux.o linux_timer.o nvparse.o notify_rc.o semaphore.o

all: libshared.so

romfs:
	$(ROMFSINST) /lib/libshared.so

clean:
	rm -f *.o libshared.so

libshared.so: $(OBJS)
	$(LD) -shared -o $@ $^
	$(STRIP) --strip-debug $@
