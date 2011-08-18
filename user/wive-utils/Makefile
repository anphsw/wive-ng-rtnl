#based utilites for all routers

UTILSLIST = ader cpu mcprobe ipget
CFLAGS += -Os

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
	for i in $(UTILSLIST); do cp -fv $$i ./filesystem/bin/$$i; done

romfs:
	rm -rf filesystem
	mkdir filesystem
	mkdir filesystem/bin
	@set -e; 
	for i in $(UTILSLIST); do $(ROMFSINST) -S $$i /bin/$$i; done
