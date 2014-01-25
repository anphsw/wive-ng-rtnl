TOPDIR=../
include $(TOPDIR)Rules.mak

CFLAGS+=$(SSP_ALL_CFLAGS) -DHAVE_CONFIG_H -Iinclude/ -O3

LIBNTFS3G=libntfs-3g.a
LIBNTFS3G_VERS=84
LIBNTFS3G_SHARED=libntfs-3g.so.$(LIBNTFS3G_VERS)

OBJS    := acls.o attrib.o attrlist.o bitmap.o bootsect.o cache.o collate.o \
	compat.o compress.o debug.o device.o dir.o efs.o index.o inode.o \
	lcnalloc.o logfile.o logging.o 	mft.o misc.o mst.o object_id.o 	\
	realpath.o reparse.o runlist.o security.o unistr.o volume.o xattrs.o \
	unix_io.o

all:	$(LIBNTFS3G)

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

shared: all
	$(LD) $(LDFLAGS) -soname=$(LIBNTFS3G_SHARED) \
		-o $(LIBNTFS3G_SHARED) --whole-archive $(LIBNTFS3G) \
		--no-whole-archive $(TOPDIR)libc/misc/internals/interp.o \
		-L$(TOPDIR)lib -lc $(LDADD_LIBFLOAT) $(LIBGCC);
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBNTFS3G_SHARED)
	$(INSTALL) -m 644 $(LIBNTFS3G_SHARED) $(TOPDIR)lib

$(LIBNTFS3G) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBNTFS3G) $(OBJS)
	$(INSTALL) -d $(TOPDIR)lib
	$(RM) $(TOPDIR)lib/$(LIBNTFS3G)
	$(INSTALL) -m 644 $(LIBNTFS3G) $(TOPDIR)lib

$(OBJS): Makefile

clean:
	$(RM) *.[oa] $(LIBNTFS3G_SHARED)*

