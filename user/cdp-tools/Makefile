CFLAGS += -D_BSD_SOURCE -D__BSD_SOURCE -D__FAVOR_BSD -DHAVE_NET_ETHERNET_H
LDLIBS += -lnet

all: cdp-send

cdp-send: cdp-send.o

clean:
	rm -f cdp-send.o cdp-send

romfs:
	$(ROMFSINST) cdp-send /bin/cdp-send
	$(ROMFSINST) config-cdp.sh /etc/scripts/config-cdp.sh
	$(ROMFSINST) -s /etc/scripts/config-cdp.sh /bin/config-cdp.sh
