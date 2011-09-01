all:
	$(MAKE) -C source

install:
	#DO NOT MAKE INSTALL NEED!!! Run make install may be crashed you system!

romfs: all
	$(ROMFSINST) -S source/bin/smbd		/bin/smbd
	$(ROMFSINST) -S source/bin/CP850.so	/lib/CP850.so
	$(ROMFSINST) -S source/bin/CP437.so	/lib/CP437.so
	$(ROMFSINST) -S smb.conf.template	/etc/smb.conf.template
	$(ROMFSINST) -S passdb.tdb		/etc/passdb.tdb
	$(ROMFSINST) -S secrets.tdb		/etc/secrets.tdb
	$(ROMFSINST) -S smbpasswd		/etc/smbpasswd
	$(ROMFSINST) -s /bin/smbd 		/bin/nmbd

clean:
	$(MAKE) -C source clean

distclean:
	$(MAKE) -C source distclean
