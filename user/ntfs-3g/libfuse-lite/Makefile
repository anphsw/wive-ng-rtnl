OBJS = fuse.o fuse_kern_chan.o fuse_loop.o fuse_lowlevel.o fuse_opt.o \
	fuse_session.o fuse_signals.o fusermount.o helper.o mount.o mount_util.o

LIBRARY = libfuse-lite.a

CFLAGS += -I.. -I../include -I../include/fuse-lite -O3
.PHONY: all clean

all: $(LIBRARY)

$(LIBRARY): $(OBJS)
	$(AR) rcu $@ $(OBJS)
	$(RANLIB) $@

clean:
	rm -f $(OBJS) $(LIBRARY)

romfs:
	#ok
