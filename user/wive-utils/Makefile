CC=mips-linux-gcc
STRIP=mips-linux-strip
UTILSLIST=ader cpu mcprobe ipget

all:
	@set -e;
	for i in $(UTILSLIST); do $(CC) $$i.c -o $$i; $(STRIP) $$i; done

clean:
	@set -e; 
	for i in $(UTILSLIST); do rm -rf $$i; done
	rm -rfd filesystem

install:
	rm -rfd filesystem
	mkdir filesystem
	mkdir filesystem/bin
	@set -e; 
	for i in $(UTILSLIST); do cp $$i ./filesystem/bin/$$i; done

romfs:
	rm -rfd filesystem
	mkdir filesystem
	mkdir filesystem/bin
	@set -e; 
	for i in $(UTILSLIST); do cp $$i $(ROMFSDIR)/bin/$$i; done
