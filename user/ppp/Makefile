# PPP top-level Makefile for Linux.
#include ../../config.arch

CFLAGS += -O2

DESTDIR = $(INSTROOT)/opt/Wive-RTNL/user/ppp/filesystem
BINDIR = $(DESTDIR)/sbin
INCDIR = $(DESTDIR)/include
MANDIR = $(DESTDIR)/share/man
ETCDIR = $(INSTROOT)/etc/ppp

# uid 0 = root
INSTALL= install

all:
	cd chat; $(MAKE) $(MFLAGS) all
	cd pppd/plugins; $(MAKE) $(MFLAGS) all
	cd pppd; $(MAKE) $(MFLAGS) all
#	cd pppstats; $(MAKE) $(MFLAGS) all
#	cd pppdump; $(MAKE) $(MFLAGS) all

install: $(BINDIR) $(MANDIR)/man8 install-progs install-devel

install-progs:
	cd chat; $(MAKE) $(MFLAGS) install
	cd pppd/plugins; $(MAKE) $(MFLAGS) install
	cd pppd; $(MAKE) $(MFLAGS) install
#	cd pppstats; $(MAKE) $(MFLAGS) install
#	cd pppdump; $(MAKE) $(MFLAGS) install

install-etcppp: $(ETCDIR) $(ETCDIR)/options $(ETCDIR)/pap-secrets \
	$(ETCDIR)/chap-secrets

install-devel:
	cd pppd; $(MAKE) $(MFLAGS) install-devel

$(ETCDIR)/options:
	$(INSTALL) -m 644 etc.ppp/options $@
$(ETCDIR)/pap-secrets:
	$(INSTALL) -m 600 etc.ppp/pap-secrets $@
$(ETCDIR)/chap-secrets:
	$(INSTALL) -m 600 etc.ppp/chap-secrets $@

$(BINDIR):
	$(INSTALL) -d -m 755 $@
$(MANDIR)/man8:
	$(INSTALL) -d -m 755 $@
$(ETCDIR):
	$(INSTALL) -d -m 755 $@

romfs:
	cp -fv ./filesystem/lib/pppd/2.4.5/* $(ROOTDIR)/romfs/lib
	cp -fv ./filesystem/sbin/* $(ROOTDIR)/romfs/bin

clean:
	rm -f `find . -name '*.[oas]' -print`
	rm -f `find . -name 'core' -print`
	rm -f `find . -name '*~' -print`
	rm -rf filesystem
	cd chat; $(MAKE) clean
	cd pppd/plugins; $(MAKE) clean
	cd pppd; $(MAKE) clean
#	cd pppstats; $(MAKE) clean
#	cd pppdump; $(MAKE) clean

dist-clean:	clean
	rm -f Makefile `find . -name Makefile -print`

#kernel:
#	cd linux; ./kinstall.sh

# no tests yet, one day...
installcheck:
	true
