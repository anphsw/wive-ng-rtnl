# xupnpd top-level include
-include ../../config.arch

DIRLIST=playlists plugins ui www

all:
	make -C src embedded

clean:
	make -C src clean

install:
	#OK

romfs:
	cp -fv src/xupnpd $(ROMFSDIR)/usr/bin/xupnpd
	rm -rf $(ROMFSDIR)/etc/xupnpd
	mkdir -p $(ROMFSDIR)/etc/xupnpd
	mkdir -p $(ROMFSDIR)/etc/xupnpd/config
	mkdir -p $(ROMFSDIR)/etc/xupnpd/xupnpd-feeds
	cp -fv src/*.lua $(ROMFSDIR)/etc/xupnpd
	for i in $(DIRLIST); do cp -apr src/$$i $(ROMFSDIR)/etc/xupnpd/$$i; done
