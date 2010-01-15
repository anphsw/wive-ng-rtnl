TOPDIR=../
include $(TOPDIR)Rules.mak

CONFOPTS= --host=mipsel-linux

LIB_LDFLAGS := $(subst -z defs,,$(LDFLAGS))


# upnp
LIBUPNP_A= libupnp.a
LIBUPNP_SHARED=libupnp.so
LIBUPNP_SHARED_FULLNAME=libupnp-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

# ixml
LIBIXML_A= libixml.a
LIBIXML_SHARED=libixml.so
LIBIXML_SHARED_FULLNAME=libixml-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so

# threadutil
LIBTHREADUTIL_A= libthreadutil.a
LIBTHREADUTIL_SHARED= libthreadutil.so
LIBTHREADUTIL_SHARED_FULLNAME= libthreadutil-$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL).so


all: Makefile
	make -f Makefile all
	cp upnp/.libs/$(LIBUPNP_A) .
	cp ixml/.libs/$(LIBIXML_A) .
	cp threadutil/.libs/$(LIBTHREADUTIL_A) .

Makefile:
	AR= AS= CC=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-gcc CFLAGS= CROSS= CROSS_COMPILE= CXX= HOSTCC= LD=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-ld OBJCOPY= RANLIB= STRIP= GCC_DIR= ./configure $(CONFOPTS)
#	CC="/opt/buildroot-gdb/bin/mipsel-linux-uclibc-gcc" ./configure $(CONFOPTS)

# the is a proxy section --YY
all-recursive: Makefile
	make -f Makefile all-recursive

# the is a proxy section --YY
all-am: Makefile
	make -f Makefile all-am

# the is a proxy section --YY
clean-am: Makefile
	make -f Makefile clean-am

clean: Makefile
	rm -rf *.so *.a
	make -f Makefile clean
	find ./ -name Makefile -exec rm -f {} \;

romfs:

shared: all
#UPNP
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBUPNP_SHARED).$(MAJOR_VERSION) \
           -o $(LIBUPNP_SHARED_FULLNAME) --whole-archive $(LIBUPNP_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBUPNP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBUPNP_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBUPNP_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBUPNP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBUPNP_SHARED)
	$(LN) -sf $(LIBUPNP_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBUPNP_SHARED).$(MAJOR_VERSION)

#IXML
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBIXML_SHARED).$(MAJOR_VERSION) \
           -o $(LIBIXML_SHARED_FULLNAME) --whole-archive $(LIBIXML_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBIXML_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBIXML_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBIXML_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBIXML_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBIXML_SHARED)
	$(LN) -sf $(LIBIXML_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBIXML_SHARED).$(MAJOR_VERSION)

#THREADUTIL
	$(LD) $(LIB_LDFLAGS) -soname=$(LIBTHREADUTIL_SHARED).$(MAJOR_VERSION) \
           -o $(LIBTHREADUTIL_SHARED_FULLNAME) --whole-archive $(LIBTHREADUTIL_A) \
           --no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
           -L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);

	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBTHREADUTIL_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBTHREADUTIL_SHARED).$(MAJOR_VERSION)
	$(INSTALL) -m 644 $(LIBTHREADUTIL_SHARED_FULLNAME) $(TOPDIR)lib
	$(LN) -sf $(LIBTHREADUTIL_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBTHREADUTIL_SHARED)
	$(LN) -sf $(LIBTHREADUTIL_SHARED_FULLNAME) $(TOPDIR)lib/$(LIBTHREADUTIL_SHARED).$(MAJOR_VERSION)

