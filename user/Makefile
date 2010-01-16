#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: config all romfs clean prune

#
# Include architecture specific build rules.
#
ifndef ROOTDIR
ROOTDIR=..
endif

UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)
-include $(MODULES_CONFIG)

VEND=$(ROOTDIR)/vendors

#
# must run the vendor build first
#
dir_v = $(VEND)/$(CONFIG_VENDOR)/$(CONFIG_PRODUCT)/.
dir_p = $(ROOTDIR)/prop

dir_y = 
dir_n =
dir_  =


dir_$(CONFIG_ASUS_SHARED)                   += shared
dir_$(CONFIG_ASUS_NVRAM)		    += nvram
dir_$(CONFIG_ASUS_NETCONF)		    += detectWAN
dir_$(CONFIG_ASUS_NETCONF)                  += netconf
dir_$(CONFIG_ASUS_RC)                       += rc
dir_$(CONFIG_ASUS_INFOSVR)                  += infosvr
dir_$(CONFIG_ASUS_NTPCLIENT)                += ntpclient.asus
dir_$(CONFIG_ASUS_HTTPD)                    += httpd
dir_$(CONFIG_ASUS_WWW)                      += www #2008.10 magic

dir_$(CONFIG_USER_MTDUTILS)                 += mtd-utils

dir_$(CONFIG_USER_GDBSERVER_GDBREPLAY)      += gdbserver
dir_$(CONFIG_USER_GDBSERVER_GDBSERVER)      += gdbserver
dir_$(CONFIG_USER_STRACE_STRACE)            += strace

dir_$(CONFIG_USER_BRCTL_BRCTL)              += bridge-utils
dir_$(CONFIG_USER_GOAHEAD_HTTPD)	    += goahead
dir_$(CONFIG_USER_NTPCLIENT_NTPCLIENT)      += ntpclient
dir_$(CONFIG_USER_NTPCLIENT_ADJTIMEX)       += ntpclient
dir_$(CONFIG_USER_NTPD_NTPDATE)             += ntp
dir_$(CONFIG_USER_WIRELESS_TOOLS)           += wireless_tools
dir_$(CONFIG_USER_IPTABLES_IPTABLES)        += iptables
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)       += iptables
dir_$(CONFIG_LLDT)			    += lldt
dir_$(CONFIG_USER_INADYN)                   += inadyn
dir_$(CONFIG_USER_OPENSSL)	   	    += openssl-0.9.8e
dir_$(CONFIG_USER_WPA_SUPPLICANT)	    += wpa_supplicant-0.4.9
dir_$(CONFIG_USER_802_1X)		    += 802.1x

dir_$(CONFIG_USER_DNSMASQ)                  += dnsmasq
dir_$(CONFIG_USER_DROPBEAR)                 += dropbear

dir_$(CONFIG_RT2880_APP)		    += rt2880_app
dir_$(CONFIG_BPALOGIN)                      += bpalogin
dir_$(CONFIG_EZ_IPUPDATE)                   += ez-ipupdate

dir_$(CONFIG_USER_MINIUPNPD)          	    += upnp
dir_$(CONFIG_USER_UPNP_IGD)                 += linux-igd
dir_$(CONFIG_USER_WSC)			    += wsc_upnp

dir_$(CONFIG_USER_TC)                  	    += iproute2
dir_$(CONFIG_USER_PPPD)                	    += pppd
dir_$(CONFIG_USER_PPPOE_RELAY)              += pppoe-relay
dir_$(CONFIG_USER_XL2TPD)		    += xl2tpd

dir_$(CONFIG_USER_IGMPPROXY)                += igmpproxy
dir_$(CONFIG_USER_UDPXY)                    += udpxy

dir_$(CONFIG_LIBUSB)                        += libusb
dir_$(CONFIG_U2EC)                          += u2ec
dir_$(CONFIG_LPRNG)                         += LPRng
# add for HSDPA support                      
dir_y$(CONFIG_USER_HSDPA)		    += usb_modeswitch-0.9.5
dir_y$(CONFIG_USER_HSDPA)		    += comgt-0.32
#dir_y$(CONFIG_USER_HSDPA)                   += sdparm-1.02
dir_y$(CONFIG_USER_HSDPA)                   += hso-1.6


#busybox need last for replace unneeded apps to busybox applets
dir_$(CONFIG_USER_BUSYBOX_BUSYBOX)          += busybox

all: config
ifeq ($(CONFIG_USER_PPPD),y)
	cd ppp && ./DoConfigure.sh && make && make install && cd ..
endif
ifeq ($(CONFIG_USER_DROPBEAR),y)
	cd dropbear  && ./DoConfigure.sh && make && make install && cd ..
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
ifeq ($(CONFIG_PPPD),y)
	cd ppp && make romfs && cd ..
endif

clean:
	-for i in $(dir_v) $(sort $(dir_y) $(dir_n) $(dir_)) $(dir_p) ; do \
		if [ -f $$i/.sgbuilt_user ]; then \
			$(MAKE) -C $$i clean ; \
			rm -f $$i/.sgbuilt_user; \
		fi; \
	done

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

