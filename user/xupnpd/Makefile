# xupnpd top-level include
-include ../../config.arch

DIRLIST_ETC=playlists plugins
DIRLIST_SHR=ui www profiles

all:
	make -C src embedded

clean:
	make -C src clean

install:
	#OK

romfs:
	cp -fv src/xupnpd $(ROMFSDIR)/bin/xupnpd
	rm -rf $(ROMFSDIR)/etc/xupnpd
	mkdir -p $(ROMFSDIR)/etc/xupnpd
	cp -fv src/*.lua $(ROMFSDIR)/etc/xupnpd
	mkdir -p $(ROMFSDIR)/etc/xupnpd/config
	mkdir -p $(ROMFSDIR)/etc/xupnpd/xupnpd-feeds
	for i in $(DIRLIST_ETC); do cp -apr src/$$i $(ROMFSDIR)/etc/xupnpd/$$i; done
	mkdir -p $(ROMFSDIR)/share/xupnpd
	for i in $(DIRLIST_SHR); do cp -apr src/$$i $(ROMFSDIR)/share/xupnpd/$$i; done
