-include $(LINUX_CONFIG)

EXEC = hw_nat

ifeq ($(CONFIG_HNAT_EXTENTIONS),y)
EXEC += acl ac mtr
endif

HWNAT_OBJS = hw_nat.o hwnat_api.o util.o
AC_OBJS = ac.o ac_api.o util.o
ACL_OBJS = acl.o acl_api.o util.o
MTR_OBJS = mtr.o mtr_api.o util.o
CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/net/nat/hw_nat

all: $(EXEC)

hw_nat: $(HWNAT_OBJS) $(CONF_H)
	$(CC) $(LDFLAGS) -o $@ $(HWNAT_OBJS)

ifeq ($(CONFIG_HNAT_EXTENTIONS),y)
acl: $(ACL_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(ACL_OBJS)

ac: $(AC_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(AC_OBJS)

mtr: $(MTR_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(MTR_OBJS)
endif

romfs:
	$(ROMFSINST) /bin/hw_nat
ifeq ($(CONFIG_HNAT_EXTENTIONS),y)
	$(ROMFSINST) /bin/acl
	$(ROMFSINST) /bin/ac
	$(ROMFSINST) /bin/mtr
endif

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
