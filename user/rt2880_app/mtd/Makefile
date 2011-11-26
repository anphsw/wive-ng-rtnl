EXEC = mtd_write
OBJS = led.o mtd.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
LDFLAGS += -static

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
