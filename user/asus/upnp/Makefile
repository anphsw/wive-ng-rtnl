#
# Linux upnp Makefile
#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.1.1.1 2007/02/15 12:15:07 jiahao Exp $
#

#UPNP	:= $(SRCBASE)/tools/upnp/igd/linux
SRCBASE = $(ROOTDIR)
UPNP	:= $(ROOTDIR)/user/upnp/igd/linux
CFLAGS	+= -s -Os
#CFLAGS	+= -DASUS_NVRAM
#DEBUG	:= 1

all: upnp

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install -m 755 upnp $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/upnp

clean:
	rm -f upnp
	rm -f igd/linux/upnp
	$(MAKE) -C $(UPNP) clean

romfs:
	cp igd/linux/upnp ./
	$(STRIP) upnp
	$(ROMFSINST) /usr/sbin/upnp

upnp: FORCE
	$(MAKE) -C $(UPNP) DEBUG=$(DEBUG) SRCBASE=$(SRCBASE) TARGET_PREFIX=$(CROSS_COMPILE)

FORCE:
