LDLIBS=
ADDLIB=

#DEFINES= -DRESOLVE_HOSTNAMES
#LDLIBS+=-lresolv

SHARED_LIBS = n

#options for decnet
#DEFINES+= -DDECNET
#ADDLIB+=dnet_ntop.o dnet_pton.o

#options for ipx
#DEFINES+= -DIPX
#ADDLIB+=ipx_ntop.o ipx_pton.o

HOSTCC = gcc
CCOPTS = -D_GNU_SOURCE -Os -Wall -ffunction-sections -fdata-sections
CFLAGS = -I../include $(DEFINES) $(CCOPTS)

LDLIBS += -L../lib -lnetlink -lutil
LDFLAGS += -Wl,--gc-sections

#SUBDIRS=lib ip tc misc #netem genl
SUBDIRS=lib tc misc

LIBNETLINK=../lib/libnetlink.a
LIBUTIL=../lib/libutil.a

all:
	@set -e; \
	for i in $(SUBDIRS); \
	do $(MAKE) $(MFLAGS) -C $$i; done

snapshot:
	echo "static const char SNAPSHOT[] = \""`date +%y%m%d`"\";" \
		> include/SNAPSHOT.h

romfs:
	$(ROMFSINST) -S tc/tc /bin/tc
#	@for i in $(SUBDIRS); \
#	do $(MAKE) $(MFLAGS) -C $$i romfs; done

clean:
	@for i in $(SUBDIRS) doc; \
	do $(MAKE) $(MFLAGS) -C $$i clean; done

.EXPORT_ALL_VARIABLES:
