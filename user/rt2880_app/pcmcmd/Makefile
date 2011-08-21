EXEC = pcmcmd

OBJS = pcmcmd.o

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char/pcm

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
