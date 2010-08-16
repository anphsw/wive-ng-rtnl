#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: config all romfs clean prune

#
# Include architecture specific build rules.
#

include ../config.arch                                                                                                       
include $(LINUX_CONFIG) 

ifndef ROOTDIR                                                                                                                              
ROOTDIR=..                                                                                                                                  
endif 

ifndef CONFIG_IPV6
CFLAGS += DNO_IPV6
endif   

CFLAGS += -Os -mips32 -mtune=mips32
CFLAGS += -Wpadded -Wall -Wno-unused 
export $(CFLAGS)

UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)
-include $(MODULES_CONFIG)

VEND=$(ROOTDIR)/vendors

#for all cleanap
SUBDIRS_APP =  goahead iptables miniupnp  rt2880_app wpa_supplicant ethtool
SUBDIRS_APP += igmpproxy lldt ppp strace wsc_upnp busybox inadyn pppoe-relay udpxy xl2tpd
SUBDIRS_APP += 802.1x dnsmasq gdbserver iproute2 wive-utils radvd wireless_tools zebra dropbear
SUBDIRS_APP += openssl-0.9.8e

#
# must run the vendor build first
#
dir_v = $(VEND)/$(CONFIG_VENDOR)/$(CONFIG_PRODUCT)/.
dir_p = $(ROOTDIR)/prop

dir_y = 
dir_n =
dir_  =

#basy system application
dir_$(CONFIG_USER_GDBSERVER_GDBREPLAY)      += gdbserver
dir_$(CONFIG_USER_GDBSERVER_GDBSERVER)      += gdbserver
dir_$(CONFIG_USER_STRACE_STRACE)            += strace
dir_$(CONFIG_USER_WIRELESS_TOOLS)           += wireless_tools
dir_$(CONFIG_USER_IPTABLES_IPTABLES)        += iptables
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)       += iptables
dir_$(CONFIG_USER_LLTD)			    += lldt
dir_$(CONFIG_USER_INADYN)                   += inadyn
dir_$(CONFIG_USER_802_1X)		    += 802.1x
dir_$(CONFIG_USER_OPENSSL)	   	    += openssl-0.9.8e
dir_$(CONFIG_USER_WPA_SUPPLICANT)	    += wpa_supplicant
dir_$(CONFIG_USER_DNSMASQ)                  += dnsmasq
dir_$(CONFIG_USER_DROPBEAR)                 += dropbear
dir_$(CONFIG_USER_TC)                  	    += iproute2
dir_$(CONFIG_USER_RADVD)               	    += radvd
dir_$(CONFIG_USER_ZEBRA)		    += zebra
dir_$(CONFIG_USER_PPPD)                	    += pppd
dir_$(CONFIG_USER_PPPPOE_RELAY)             += pppoe-relay
dir_$(CONFIG_USER_XL2TPD)		    += xl2tpd
dir_$(CONFIG_USER_IGMP_PROXY)               += igmpproxy
dir_$(CONFIG_USER_ETHTOOL)                  += ethtool
dir_$(CONFIG_USER_UDPXY)                    += udpxy
dir_$(CONFIG_USER_WIVE)			    += wive-utils
dir_$(CONFIG_USER_WSC)                	    += wsc_upnp
dir_$(CONFIG_USER_MINIUPNPD)                += miniupnp

#only ralink apps and scripts
dir_$(CONFIG_RT2880_APP)		    += rt2880_app

#need last busybox and goahead
dir_$(CONFIG_USER_BUSYBOX_BUSYBOX)          += busybox
dir_$(CONFIG_USER_SAMBA)		    += samba
dir_$(CONFIG_USER_GOAHEAD_HTTPD)	    += goahead

#alwayse make service utility
dir_y                                       += getBoardID

all: config
ifeq ($(CONFIG_USER_PPPD),y)
	cd radvd && ./configure-command && cd ..
endif
ifeq ($(CONFIG_USER_PPPD),y)
	cd ppp && ./DoConfigure.sh && make && make install && cd ..
endif
ifeq ($(CONFIG_USER_DROPBEAR),y)
	cd dropbear  && ./DoConfigure.sh && make && make install && cd ..
endif
ifeq ($(CONFIG_USER_ZEBRA),y)
	cd zebra  && ./DoConfigure.sh && make && make install && cd ..
endif
ifeq ($(CONFIG_USER_ETHTOOL),y)
	cd ethtool  && ./DoConfigure.sh && make && make install && cd ..
endif
	$(MAKE) -j$(HOST_NCPU) $(dir_y) $(dir_v) $(dir_p) || exit $$?	

#
# add directory dependancies here
#
.PHONY: $(sort $(dir_y) $(dir_p))

$(sort $(dir_y) $(dir_p)):  config
	[ ! -d "$@" ] || ( touch $@/.sgbuilt_user && $(MAKE) -j1 -C $@ ) || exit $$?

%_only:
	touch $(@:_only=)/.sgbuilt_user && $(MAKE) -j1 -C $(@:_only=)

%_clean:
	$(MAKE) -j1 -C $(@:_clean=) clean; rm -f $(@:_clean=)/.sgbuilt_user; true

romfs:
	for i in $(sort $(dir_y)) $(dir_p) ; do \
		[ ! -d $$i ] || $(MAKE) -C $$i romfs || exit $$? ; \
	done
ifeq ($(CONFIG_USER_PPPD),y)
	cd ppp && make romfs && cd ..
endif
ifeq ($(CONFIG_USER_MINIUPNPD),y)
	cd miniupnp  && ./genconfig.sh && make && make install && cd ..
endif

clean:
	-for i in $(dir_v) $(sort $(dir_y) $(dir_n) $(dir_)) $(dir_p) ; do \
		if [ -f $$i/.sgbuilt_user ]; then \
			$(MAKE) -C $$i clean ; \
			rm -f $$i/.sgbuilt_user; \
		fi; \
	done
	echo "Full cleanUP"
	@set -e; for i in $(SUBDIRS_APP); do \
	$(MAKE) -C $$i clean; rm -rfd $$i/filesystem/*; \
	done
	make -C busybox mrproper

prune:
	-for i in $(sort $(dir_n) $(dir_)) ; do \
		found=0; \
		for j in $(sort $(dir_y)) ; do \
			if [ $$i == $$j ]; then \
				found=1; \
			fi; \
		done; \
		if [ $$found == 0 ]; then \
			[ "$$i" = "freeswan" ] && make prune_freeswan; \
			[ "$$i" = "openswan" ] && make prune_openswan; \
			rm -fr $$i; \
		fi; \
	done

prune_freeswan:
	@mkdir $(ROOTDIR)/$@
	@mv $(ROOTDIR)/freeswan/klips/net/ipsec/Config.in $(ROOTDIR)/$@/
	@rm -fr $(ROOTDIR)/freeswan
	@mkdir -p $(ROOTDIR)/freeswan/klips/net/ipsec
	@mv $(ROOTDIR)/$@/* $(ROOTDIR)/freeswan/klips/net/ipsec/
	@rm -fr $(ROOTDIR)/$@

prune_openswan:
	@mkdir $(ROOTDIR)/$@
	@mv $(ROOTDIR)/openswan/linux/net/ipsec/{Config.in*,Kconfig} $(ROOTDIR)/$@/
	@rm -fr $(ROOTDIR)/openswan
	@mkdir -p $(ROOTDIR)/openswan/linux/net/ipsec
	@mv $(ROOTDIR)/$@/* $(ROOTDIR)/openswan/linux/net/ipsec/
	@rm -fr $(ROOTDIR)/$@

web:
	$(MAKE) -C goahead all
