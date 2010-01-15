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
dir_y			                    += libdisk
dir_$(CONFIG_USER_BRCTL_BRCTL)              += bridge-utils
dir_$(CONFIG_USER_BUSYBOX_BUSYBOX)          += busybox
dir_$(CONFIG_USER_GDBSERVER_GDBREPLAY)      += gdbserver
dir_$(CONFIG_USER_GDBSERVER_GDBSERVER)      += gdbserver
dir_$(CONFIG_USER_GOAHEAD_HTTPD)	    += goahead
dir_$(CONFIG_USER_NTPCLIENT_NTPCLIENT)      += ntpclient
dir_$(CONFIG_USER_NTPCLIENT_ADJTIMEX)       += ntpclient
dir_$(CONFIG_USER_NTPD_NTPDATE)             += ntp
dir_$(CONFIG_USER_MTDUTILS)                 += mtd-utils

dir_$(CONFIG_USER_STRACE_STRACE)            += strace
dir_$(CONFIG_USER_UPNP_IGD)                 += linux-igd
dir_$(CONFIG_USER_WIRELESS_TOOLS)           += wireless_tools
dir_$(CONFIG_USER_IPTABLES_IPTABLES)        += iptables
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)       += iptables

#Added by Steven
dir_$(CONFIG_LMBENCH3)			    += lmbench3
dir_$(CONFIG_USER_XL2TPD)		    += xl2tpd
dir_$(CONFIG_LLDT)			    += lldt
dir_$(CONFIG_USER_IXIA_ENDPOINT)	    += ixia_endpoint

#--
#Added by YY
dir_$(CONFIG_USER_INADYN)                   += inadyn
dir_$(CONFIG_USER_OPENSSL)	   	    += openssl-0.9.8e
ir_$(CONFIG_USER_WPA_SUPPLICANT)	    += wpa_supplicant-0.4.9
dir_$(CONFIG_USER_WSC)			    += wsc_upnp
dir_$(CONFIG_USER_802_1X)		    += 802.1x
#--

#Jiahao
dir_$(CONFIG_ASUS_NETCONF)                  += netconf
dir_$(CONFIG_ASUS_RC)                       += rc
dir_$(CONFIG_DPROXY)                  	    += dproxy
dir_$(CONFIG_ASUS_INFOSVR)                  += infosvr
dir_$(CONFIG_RT2880_APP)		    += rt2880_app
dir_y                			    += detectWAN #2008.10 magic
dir_$(CONFIG_ASUS_HTTPD)                    += httpd
dir_$(CONFIG_TC)                   	    += iproute2
dir_$(CONFIG_UDHCPD)                   	    += udhcpd
dir_$(CONFIG_BPALOGIN)                      += bpalogin
dir_$(CONFIG_ASUS_NTPCLIENT)                += ntpclient.asus
dir_$(CONFIG_UPNP)                	    += upnp
dir_$(CONFIG_EZ_IPUPDATE)                   += ez-ipupdate
dir_$(CONFIG_ASUS_WWW)                      += www #2008.10 magic
dir_$(CONFIG_L2TP)                          += rp-l2tp
dir_$(CONFIG_VSFTPD)                        += vsftpd
dir_$(CONFIG_IGMPPROXY)                     += igmpproxy
dir_$(CONFIG_LIBUSB)                        += libusb
dir_$(CONFIG_U2EC)                          += u2ec
dir_$(CONFIG_LPRNG)                         += LPRng

dir_$(CONFIG_USER_PPPD)                     += ppp-2.4.2	# from SDK 2.1, not used in n13u
dir_$(CONFIG_USER_PPPD_PPPSTATS)            += ppp-2.4.2	# from SDK 2.1
dir_$(CONFIG_USER_PPPD_PPPDUMP)             += ppp-2.4.2	# from SDK 2.1
dir_$(CONFIG_PPPD)                	    += pppd
dir_$(CONFIG_PPTP_CLIENT)                   += pptp-client
dir_$(CONFIG_PPPOE_RELAY)                   += pppoe-relay

# add for HSDPA support                      
dir_y$(CONFIG_USER_HSDPA)		    += usb_modeswitch-0.9.5
dir_y$(CONFIG_USER_HSDPA)		    += comgt-0.32
#dir_y$(CONFIG_USER_HSDPA)                   += sdparm-1.02
dir_y$(CONFIG_USER_HSDPA)                   += hso-1.6


all: config
#	$(MAKE) -j$(HOST_NCPU) $(sort $(dir_y) $(dir_v) $(dir_p)) || exit $$?
	$(MAKE) -j$(HOST_NCPU) $(dir_y) $(dir_v) $(dir_p) || exit $$?	
#	$(MAKE) -DWEBSTRFILTER -j$(HOST_NCPU) $(dir_y) $(dir_v) $(dir_p) || exit $$?	

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

