EXEC=igmpproxy
IGMP_PROXY_OBJ= igmpproxy.o config.o confread.o request.o mcgroup.o rttable.o \
		igmp.o ifvc.o callout.o kern.o syslog.o lib.o mroute-api.o

-include $(LINUX_CONFIG)

CFLAGS += -I$(ROOTDIR)/$(LINUXDIR) -I$(ROOTDIR)/$(LINUXDIR)/include
CONF_H = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf

#CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT
#CFLAGS+= -DSEARCH_CLIENT_IN_WIFI_MACTABLE

ifdef CONFIG_RALINK_RT3052
ifdef CONFIG_RT_3052_ESW
IGMP_PROXY_OBJ+= rt3052.o rt3052_reg.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT
endif
endif

ifdef CONFIG_RALINK_RT3352
ifdef CONFIG_RT_3052_ESW
IGMP_PROXY_OBJ+= rt3052.o rt3052_reg.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT
endif
endif

ifdef CONFIG_RALINK_RT5350
ifdef CONFIG_RT_3052_ESW
IGMP_PROXY_OBJ+= rt3052.o rt3052_reg.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT
endif
endif

ifdef CONFIG_RALINK_RT6855
ifdef CONFIG_RT_3052_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o rt3052_reg.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT
endif
endif

ifdef CONFIG_RALINK_RT6855A
ifdef CONFIG_RT_3052_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o rt3052_reg.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT
endif
endif

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RT_3052_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o rt3052_reg.o
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT
endif
endif

all: $(EXEC)

igmpproxy: build.h $(IGMP_PROXY_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(IGMP_PROXY_OBJ)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	rm -f *.o *.asm build.h $(EXEC)

build.h	:
	echo '#define BUILD "' `date +%y%m%d` '"' >build.h
