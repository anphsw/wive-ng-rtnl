#
# Linux netconf Makefile
#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.1.1.1 2007/02/15 12:14:15 jiahao Exp $
#

CFLAGS	= -I. -I$(ROOTDIR)/linux-2.6.21.x/include -I$(ROOTDIR)/user/shared/include -I$(ROOTDIR)/user/shared -I$(ROOTDIR)/user/iptables/include -I$(ROOTDIR)/user/iptables/libiptc
CFLAGS	+= -DASUS_NVRAM -s
#LDFLAGS += -L. -L$(ROOTDIR)/user/iptables/libiptc
LDFLAGS = -L. -L$(ROOTDIR)/user/iptables/libiptc

all: libnetconf.so

romfs:
	$(STRIP) libnetconf.so
	$(ROMFSINST) /lib/libnetconf.so

clean:
	rm -f *.o *.a *.so netconf

libnetconf.so: netconf_linux.o
	$(LD) -shared -o $@ $^ $(LDFLAGS) -liptc

netconf: main.o
	$(CC) -o $@ $< $(LDFLAGS) -lnetconf
