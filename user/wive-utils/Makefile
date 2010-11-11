#baswd utility for all routers

UTILSLIST = ader cpu mcprobe ipget
CFLAGS += -Os

ifdef CONFIG_USER_KABINET
UTILSLIST += lanauth
CFLAGS += -lcrypt
endif

all:
	@set -e;
	for i in $(UTILSLIST); do $(CC) $(CFLAGS) $$i.c -o $$i; $(STRIP) $$i; done

clean:
	@set -e; 
	for i in $(UTILSLIST); do rm -rf $$i; done
	rm -rf filesystem

install:
	rm -rf filesystem
	mkdir filesystem
	mkdir filesystem/bin
	@set -e; 
	for i in $(UTILSLIST); do cp $$i ./filesystem/bin/$$i; done

romfs:
	rm -rf filesystem
	mkdir filesystem
	mkdir filesystem/bin
	@set -e; 
	for i in $(UTILSLIST); do cp $$i $(ROMFSDIR)/bin/$$i; done
