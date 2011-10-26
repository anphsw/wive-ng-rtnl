## Process this file with automake to produce Makefile.in.

SUBDIRS = lib @ZEBRA@ @BGPD@ @RIPD@ @RIPNGD@ @OSPFD@ @OSPF6D@ @VTYSH@ doc

EXTRA_DIST = aclocal.m4 SERVICES TODO REPORTING-BUGS vtysh/Makefile.in \
	vtysh/Makefile.am update-autotools

dist-hook:
	mkdir $(distdir)/tools
	cp -p $(srcdir)/tools/*.pl $(distdir)/tools
	cp -p $(srcdir)/tools/*.el $(distdir)/tools
	cp -p $(srcdir)/tools/*.cgi $(distdir)/tools
	mkdir $(distdir)/init
	mkdir $(distdir)/init/redhat
	cp -p $(srcdir)/init/redhat/*.init $(distdir)/init/redhat
	cp -p $(srcdir)/init/redhat/zebra.* $(distdir)/init/redhat

romfs: install
	$(ROMFSINST) -S init.d/W46ripd /etc/rc.d/W46ripd
	$(ROMFSINST) -s /etc/rc.d/W46ripd /etc/init.d/ripd
	$(ROMFSINST) -S init.d/W47zebra /etc/rc.d/W47zebra
	$(ROMFSINST) -s /etc/rc.d/W47zebra /etc/init.d/zebra
	$(ROMFSINST) -S filesystem/sbin/zebra /bin/zebra
	$(ROMFSINST) -S filesystem/sbin/ripd /bin/ripd
