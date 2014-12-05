LDLIBS=
ADDLIB=

DEFINES=
#DEFINES+=-DRESOLVE_HOSTNAMES
#LDLIBS+=-lresolv
#NO_SHARED_LIBS = y

SUBDIRS=lib tc ip

HOSTCC = gcc
CCOPTS = -D_GNU_SOURCE -Os -Wall -fno-caller-saves -fno-reorder-blocks -fno-unwind-tables -fno-asynchronous-unwind-tables -femit-struct-debug-baseonly -fno-var-tracking
CCOPTS += -ffunction-sections -fdata-sections
CFLAGS =  $(DEFINES) $(CCOPTS) -I../include -I$(ROOTDIR)/lib/include
YACCFLAGS = -d -t -v

LDLIBS += -L../lib -lnetlink -lutil
LDFLAGS += -Wl,--gc-sections

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
ifdef ROMFS
	rm -f $(ROMFS)/bin/ip
	rm -f $(ROMFS)/bin/tc
endif
	$(ROMFSINST) -S -e CONFIG_USER_IPROUTE2_TC tc/tc /bin/tc
	$(ROMFSINST) -S -e CONFIG_USER_IPROUTE2_IP ip/ip /bin/ip

clean:
	@for i in $(SUBDIRS) doc; \
	do $(MAKE) $(MFLAGS) -C $$i clean; done

.EXPORT_ALL_VARIABLES:
