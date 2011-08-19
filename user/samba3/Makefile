all:
	$(MAKE) -C source

romfs: all
	$(ROMFSINST) -S source/bin/nmbd /sbin/nmbd
	$(ROMFSINST) -S source/bin/smbd /sbin/smbd
	$(ROMFSINST) -S source/bin/smbpasswd /sbin/smbpasswd
	$(ROMFSINST) -S source/bin/CP850.so /lib/CP850.so
	$(ROMFSINST) -S source/bin/CP437.so /lib/CP437.so
	$(ROMFSINST) -S smb.conf.template /etc/smb.conf.template

clean:
	$(MAKE) -C source clean
