
.EXPORT_ALL_VARIABLES:

CFLAGS    += -ffunction-sections -fdata-sections
CXXFLAGS  += -ffunction-sections -fdata-sections
LDFLAGS   += -Wl,--gc-sections

DIRS = source

all:
	for i in $(DIRS) ; do make -C $$i || exit $? ; done

romfs:
	for i in $(DIRS) ; do make -C $$i romfs || exit $? ; done
	$(ROMFSINST) smb.conf.template /etc/smb.conf.template
	$(ROMFSINST) -s /bin/smbd /bin/nmbd

clean:
	-for i in $(DIRS) ; do make -C $$i clean ; done
