
BIN = crc_gen
INSTALLDIR = $(ROOTDIR)/romfs

all: ${BIN}
	#${STRIP} ${BIN}

crc_gen: crc_gen.c
	gcc -o crc_gen crc_gen.c

romfs:
	#$(ROMFSINST) /usr/sbin/$(BIN)
	echo "prepare dlm apps"
	echo $(INSTALLDIR)
	rm -Rf $(INSTALLDIR)/apps
	mkdir $(INSTALLDIR)/apps
	mkdir $(INSTALLDIR)/apps/bin
	tar -zxvf exinstall.tgz
	#cp -f exinstall/bin/dms $(INSTALLDIR)/apps/bin
	#cp -Rf exinstall exinstall.tmp
	#rm -Rf exinstall.tmp/bin/dms
	cd exinstall && find > $(INSTALLDIR)/apps/list && cd ../
	./crc_gen $(INSTALLDIR)/apps/list $(INSTALLDIR)/apps/crc exinstall
	cp $(INSTALLDIR)/apps/list exinstall
	cp $(INSTALLDIR)/apps/crc exinstall
	rm -f apps.tgz
	tar -zcvf apps.tgz exinstall
	#rm -Rf exinstall.tmp
	echo "make dlm crc done"

clean:
	-rm -f *.o crc_gen a.out
	rm -Rf $(INSTALLDIR)/apps
	rm -f apps.tgz
	#rm -Rf exinstall.tmp
	rm -Rf exinstall
