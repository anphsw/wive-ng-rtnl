
.EXPORT_ALL_VARIABLES:

DIRS = source

all:
	for i in $(DIRS) ; do make -C $$i || exit $? ; done

romfs:
	for i in $(DIRS) ; do make -C $$i romfs || exit $? ; done
	$(ROMFSINST) smb.conf.template /etc/smb.conf.template
	$(ROMFSINST) -s /bin/smbd /bin/nmbd

clean:
	-for i in $(DIRS) ; do make -C $$i clean ; done
