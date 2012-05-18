EXEC = switch
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/config.h

ifdef CONFIG_RALINK_RT6855
SRC=switch_gsw.c
else
ifdef CONFIG_RTL8367M
SRC=rtl8367m.c
else
SRC=switch.c
endif
endif

all: $(EXEC)
switch: $(SRC) $(CONF_H)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $@.c $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
