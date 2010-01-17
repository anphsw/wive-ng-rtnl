#
# milli_httpd Makefile
#
# Copyright (C) 2001 Broadcom Corporation
#
# $Id: Makefile,v 1.1.1.1 2008/07/21 09:17:45 james26_jang Exp $
#

USERDIR = $(ROOTDIR)/user
HTTPDIR = $(ROOTDIR)/user/asus/httpd

CFLAGS  += -I. -I$(USERDIR)/shared/include -I$(HTTPDIR)/ -I$(ROOTDIR)/linux-2.6.21.x/include -Wall
CFLAGS  += -Dlinux -DASUS_NVRAM -DNO_PARALLEL -DPRNINFO
LDFLAGS = -L$(USERDIR)/shared -L$(USERDIR)/asus/netconf -lshared -lnetconf

all: infosvr

clean:
	rm -f *.o *~ infosvr

install: all
# Neo
#	install -d $(INSTALLDIR)/usr/sbin
#	install httpd $(INSTALLDIR)/usr/sbin
#	$(STRIP) $(INSTALLDIR)/usr/sbin/httpd

	install -d $(INSTALLDIR)/usr/sbin
	$(STRIP) infosvr
	install infosvr $(INSTALLDIR)/usr/sbin

romfs:
	$(ROMFSINST) /usr/sbin/infosvr

infosvr: ./infosvr.o ./wlutils.o ./common.o ./packet.o ./service.o ./interface.o ./storage.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)
	$(STRIP) $@

