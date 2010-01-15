
PPPD_DIR = $(ROOTDIR)/user/pppd
INSTALLDIR = $(ROOTDIR)/romfs

all: pppd chat

pppd:
	$(MAKE) -C pppd_src

chat:
	$(MAKE) -C chat_src
	

romfs:	pppd_src pppd_src/plugins
	$(MAKE) -C pppd_src/plugins
	cp pppd_src/pppd ./
	cp chat_src/chat ./
	$(STRIP) pppd
	$(STRIP) chat
	$(ROMFSINST) /usr/sbin/pppd
	$(ROMFSINST) /usr/sbin/chat
	cp pppd_src/plugins/rp-pppoe/rp-pppoe.so ./
	$(STRIP) rp-pppoe.so
	$(ROMFSINST) /usr/lib/pppd/rp-pppoe.so
	cp pppd_src/plugins/pppol2tp.so ./
	$(STRIP) pppol2tp.so
	$(ROMFSINST) /usr/lib/pppd/pppol2tp.so
	ln -sf /etc/3g $(ROMFSDIR)/etc_ro/ppp/peers/3g

clean:
	rm -f pppd rp-pppoe.so pppol2tp.so chat
	$(MAKE) -C pppd_src clean
	$(MAKE) -C chat_src clean
