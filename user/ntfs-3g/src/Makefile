OBJS_probe = ntfs-3g.probe.o utils.o
OBJS_ntfs = ntfs-3g.o utils.o

TARGET_probe = ntfs-3g.probe
TARGET_ntfs = ntfs-3g

CFLAGS += -I.. -I../include -I../include/ntfs-3g -I../include/fuse-lite
EXTRALIBS += ../libfuse-lite/libfuse-lite.a -lntfs-3g -lpthread -lrt

.PHONY: all clean

all: $(TARGET_probe) $(TARGET_ntfs)
$(TARGET_probe): $(OBJS_probe)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_probe) $(EXTRALIBS) $(LDLIBS$(LDLIBS_$@))

$(TARGET_ntfs): $(OBJS_ntfs)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_ntfs) $(EXTRALIBS) $(LDLIBS$(LDLIBS_$@))
	

clean:
	rm -f *.o $(TARGET_probe) $(TARGET_ntfs)
	

romfs:
	$(ROMFSINST) /bin/$(TARGET_ntfs)
