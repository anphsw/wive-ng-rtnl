#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: config all romfs clean

#
# Include architecture specific build rules.
#

UCLINUX_BUILD_USER=1

-include $(ARCH_CONFIG)
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(MODULES_CONFIG)

ifdef CONFIG_RALINK_RT3052
-include ../vendors/Ralink/RT3052/config.arch
endif

ifndef ROOTDIR
ROOTDIR=..
endif

VEND=$(ROOTDIR)/vendors

ifndef CONFIG_IPV6
CFLAGS += -DNO_IPV6
else
CFLAGS += -DO_IPV6=1
endif

export $(CFLAGS)

#for all cleanap
SUBDIRS_APP =  goahead iptables miniupnp  rt2880_app wpa_supplicant ethtool
SUBDIRS_APP += igmpproxy lldt ppp strace wsc_upnp busybox inadyn pppoe-relay udpxy xl2tpd
SUBDIRS_APP += 802.1x dnsmasq gdbserver iproute2 wive-utils kabinet radvd wireless_tools zebra dropbear
SUBDIRS_APP += ntfs-3g p910nd transmission openssl minidlna samba samba3 cdp-tools
#
# must run the vendor build first
#
dir_v = $(VEND)/$(CONFIG_VENDOR)/$(CONFIG_PRODUCT)/.

dir_y =
dir_n =
dir_  =

#base system application
dir_$(CONFIG_USER_GDBSERVER_GDBREPLAY)      += gdbserver
dir_$(CONFIG_USER_GDBSERVER_GDBSERVER)      += gdbserver
dir_$(CONFIG_USER_STRACE_STRACE)            += strace
dir_$(CONFIG_USER_WIRELESS_TOOLS)           += wireless_tools
dir_$(CONFIG_USER_IPTABLES_IPTABLES)        += iptables
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)       += iptables
dir_$(CONFIG_USER_CDP)			    += cdp-tools
dir_$(CONFIG_USER_LLTD)			    += lldt
dir_$(CONFIG_USER_INADYN)                   += inadyn
dir_$(CONFIG_USER_OPENSSL)	   	    += openssl
dir_$(CONFIG_USER_WPA_SUPPLICANT)	    += wpa_supplicant
dir_$(CONFIG_USER_802_1X)		    += 802.1x
dir_$(CONFIG_USER_DNSMASQ)                  += dnsmasq
dir_$(CONFIG_USER_DROPBEAR)                 += dropbear
dir_$(CONFIG_USER_TC)                  	    += iproute2
dir_$(CONFIG_USER_PPPD)                	    += ppp
dir_$(CONFIG_USER_XL2TPD)		    += xl2tpd
dir_$(CONFIG_USER_PPPPOE_RELAY)             += pppoe-relay
dir_$(CONFIG_USER_PARPROUTED)		    += parprouted
dir_$(CONFIG_USER_IGMP_PROXY)               += igmpproxy
dir_$(CONFIG_USER_ETHTOOL)                  += ethtool
dir_$(CONFIG_USER_UDPXY)                    += udpxy
dir_$(CONFIG_USER_WIVE)			    += wive-utils
dir_$(CONFIG_USER_KABINET)		    += kabinet
dir_$(CONFIG_USER_MINIUPNPD)                += miniupnp
dir_$(CONFIG_USER_RADVD)               	    += radvd
dir_$(CONFIG_USER_ZEBRA)		    += zebra
dir_$(CONFIG_USER_SNMPD)                    += snmpd
dir_$(CONFIG_USER_CHILLISPOT)               += chillispot
dir_$(CONFIG_USER_TCPDUMP)		    += tcpdump

#for usb support
dir_$(CONFIG_USB_MODESWITCH)                += usb_modeswitch
dir_$(CONFIG_USER_P910ND)		    += p910nd
dir_$(CONFIG_USER_NTFS3G)		    += ntfs-3g
dir_$(CONFIG_USER_SAMBA)		    += samba
dir_$(CONFIG_USER_SAMBA3)		    += samba3
dir_$(CONFIG_USER_MINIDLNA)		    += minidlna
dir_$(CONFIG_USER_TRANSMISSION)		    += transmission
dir_$(CONFIG_USER_STORAGE)		    += usb

#wifi simle config
dir_$(CONFIG_USER_WSC)                	    += wsc_upnp

#only ralink apps and scripts
dir_$(CONFIG_RT2880_APP)		    += rt2880_app

#need last busybox and goahead
dir_$(CONFIG_USER_BUSYBOX_BUSYBOX)          += busybox
dir_$(CONFIG_USER_GOAHEAD_HTTPD)	    += goahead


all: config
	CC=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-gcc ; \
	GCC=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-gcc ; \
	CXX=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-g++ ; \
	AR=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-ar ; \
	AS=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-as ; \
	LD=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-ld ; \
	NM=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-nm ; \
	RANLIB=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-ranlib ; \
	STRIP=$(CONFIG_CROSS_COMPILER_PATH)/mipsel-linux-uclibc-strip ;
ifeq ($(CONFIG_USER_OPENSSL),y)
	# ">>>>>>>>>>>>>>>> CONFIGURE SSL NEEDED  <<<<<<<<<<<<<<<<<<<<<<"
	cd openssl ; ./Configure RT2880 no-ripemd; $(MAKE) ; \
	cd ..
endif
	# ">>>>>>>>>>>>>>>> CONFIGURE ALL NEEDED  <<<<<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -f "$$i/DoConfigure.sh" ]; then ( cd $$i; ./DoConfigure.sh; cd .. ) fi \
	done
	# ">>>>>>>>>>>>>>>>  MAKE USERSPACE  <<<<<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -d "$$i" ]; then ( cd $$i; make -j$(HOST_NCPU); cd .. ) fi \
	done

#
# add directory dependancies here
#
.PHONY: $(sort $(dir_y))

$(sort $(dir_y)):  config
	[ ! -d "$@" ] || ( touch $@/.sgbuilt_user && $(MAKE) -C $@ ) || exit $$?

%_only:
	touch $(@:_only=)/.sgbuilt_user && $(MAKE) -C $(@:_only=)

%_clean:
	$(MAKE) -C $(@:_clean=) clean; rm -f $(@:_clean=)/.sgbuilt_user; true

romfs:
	for i in $(sort $(dir_y)) ; do \
		[ ! -d $$i ] || $(MAKE) -C $$i romfs || exit $$? ; \
	done

clean:
	# ">>>>>>>>>>>>>>>> START CLEANUP <<<<<<<<<<<<<<<<<<<<<<"
	-for i in $(dir_v) $(sort $(dir_y) $(dir_n) $(dir_)) ; do \
		$(MAKE) -C $$i clean ; \
		rm -f $$i/.sgbuilt_user; \
	done
	# ">>>>>>>>>>>>>>>> FULL CLEANUP  <<<<<<<<<<<<<<<<<<<<<<"
	-for i in $(sort $(dir_y) $(dir_n) $(dir_)) ; do [ ! -d $$i ] || $(MAKE) -C $$i mrproper; done
	-for i in $(sort $(dir_y) $(dir_n) $(dir_)) ; do [ ! -d $$i ] || $(MAKE) -C $$i dist-clean; done
	-for i in $(sort $(dir_y) $(dir_n) $(dir_)) ; do [ ! -d $$i ] || $(MAKE) -C $$i distclean; done
	-@set -e; for i in $(SUBDIRS_APP); do $(MAKE) -C $$i clean; rm -rf $$i/filesystem/*; done

web:
	$(MAKE) -C goahead all
