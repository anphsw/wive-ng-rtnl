ACLOCAL_AMFLAGS = -I m4

if BUILD_CLI
  CLI_DIR = cli
endif
if BUILD_DAEMON
if !WIN32
  DAEMON_DIR = daemon
endif
endif
if BUILD_GTK
  GTK_DIR = gtk po
endif
if BUILD_MAC
  MAC_DIR = macosx
endif

SUBDIRS = \
  extras \
  third-party \
  libtransmission \
  utils \
  $(DAEMON_DIR) \
  $(CLI_DIR) \
  $(GTK_DIR) \
  $(MAC_DIR) \
  web

EXTRA_DIST = \
  qt \
  NEWS \
  AUTHORS \
  COPYING \
  README \
  autogen.sh \
  update-version-h.sh \
  Transmission.xcodeproj/project.pbxproj

romfs: install
	$(ROMFSINST) -S filesystem/bin/transmission-daemon /bin/transmission-daemon
	cp -rfva filesystem/share/transmission* $(ROMFSDIR)/share/transmission/

dist-hook:
	rm -rfa `find $(distdir)/qt -name .svn`


DISTCLEANFILES = \
  intltool-extract \
  intltool-merge \
  intltool-update 
