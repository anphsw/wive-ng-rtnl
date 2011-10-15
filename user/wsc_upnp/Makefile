#############################################################
# Makefile for WSC upnp module
#############################################################

ifndef ROOTDIR
ROOTDIR = ../..
export ROOTDIR
endif

LIB_PATH = $(ROOTDIR)/lib/lib

CROSS_COMPILE = mipsel-linux-
TARGET_HOST = mipsel-linux
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip
WIRELESS_H_INCLUDE_PATH = $(ROOTDIR)/$(LINUXDIR)/include/

PWD = $(shell pwd)

INCLUDE_PATH += -I. -I$(ROOTDIR)/lib/include_shared/upnp
INCLUDE_PATH += -I$(WIRELESS_H_INCLUDE_PATH)

INCLUDE_LIBS := --static -L$(ROOTDIR)/lib/lib -lupnp -L$(ROOTDIR)/lib/lib -lixml -L$(ROOTDIR)/lib/lib -lthreadutil -L$(ROOTDIR)/lib/lib -lpthread

OBJS = wsc_main.o wsc_common.o wsc_ioctl.o wsc_netlink.o wsc_upnp_device.o wsc_upnp_cp.o sample_util.o wsc_msg.o

EXEC = wscd

CFLAGS += -Os -Wall -D_REENTRANT $(INCLUDE_PATH)
CFLAGS += -DMULTIPLE_CARD_SUPPORT
#CFLAGS += -DRT_DEBUG

all: $(EXEC)

$(EXEC): libupnp $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)  $(INCLUDE_LIBS)
	$(STRIP) -s $(EXEC)

libupnp:
	cp $(ROOTDIR)/lib/lib/libupnp.a .
	cp $(ROOTDIR)/lib/lib/libixml.a .
	cp $(ROOTDIR)/lib/lib/libthreadutil.a .
	cp $(ROOTDIR)/lib/lib/libupnp.so .
	cp $(ROOTDIR)/lib/lib/libixml.so .
	cp $(ROOTDIR)/lib/lib/libthreadutil.so .


.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~ *.bak *.so *.a $(EXEC)
	make -C $(LIBUPNP_LIB_PATH) clean

cleanall: clean
	rm -f *.a
	rm -f *.so

romfs:
	$(ROMFSINST) -S wscd /bin/wscd
	$(ROMFSINST) -S xml/WFADeviceDesc.xml /xml/WFADeviceDesc.xml
	$(ROMFSINST) -S xml/WFAWLANConfigSCPD.xml /xml/WFAWLANConfigSCPD.xml
