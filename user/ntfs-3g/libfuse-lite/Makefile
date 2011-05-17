OBJS = fuse.o fuse_kern_chan.o fuse_loop.o fuse_loop_mt.o fuse_lowlevel.o fuse_mt.o \
	fuse_opt.o fusermount.o fuse_session.o fuse_signals.o helper.o mount.o mount_util.o

LIBRARY = libfuse-lite.a

CFLAGS += -I.. -I../include -I../include/fuse-lite -Os
.PHONY: all clean

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@
	
clean:
	rm -f $(OBJS) $(LIBRARY)
	
romfs: