TOPDIR=../
include $(TOPDIR)Rules.mak

CFLAGS+=-DHAVE_CONFIG_H -Ilibusb

LIBUSB=libusb.a
LIBUSB_VERS=0.1.12
LIBUSB_SHARED=libusb.so
LIBUSB_SHARED_FULLNAME=$(LIBUSB_SHARED).$(LIBUSB_VERS)

OBJS    := descriptors.o  error.o  linux.o  usb.o

all: $(LIBUSB)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBUSB_SHARED_FULLNAME) \
		-o $(LIBUSB_SHARED_FULLNAME) --whole-archive $(LIBUSB) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBUSB_SHARED_FULLNAME)
	$(INSTALL) -m 644 $(LIBUSB_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBUSB_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBUSB_SHARED)

$(LIBUSB) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBUSB) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBUSB)
	$(INSTALL) -m 644 $(LIBUSB) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBUSB_SHARED_FULLNAME)* 
