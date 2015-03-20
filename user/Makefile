#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: all romfs clean

#
# Include architecture specific build rules.
#

UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

# for safe cleanup
ifndef ROOTDIR
ROOTDIR=..
endif

VEND=$(ROOTDIR)/vendors

#for all cleanap
SUBDIRS_APP = $(shell LC_ALL=C; ls)

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
dir_$(CONFIG_USER_WIRELESS_IW_TOOL)         += iw
dir_$(CONFIG_USER_IPTABLES_IPTABLES)        += iptables
dir_$(CONFIG_USER_IPTABLES_IP6TABLES)       += iptables
dir_$(CONFIG_USER_CDP)			    += cdp-tools
dir_$(CONFIG_USER_LLTD)			    += lldt
dir_$(CONFIG_USER_INADYN)                   += inadyn
dir_$(CONFIG_USER_WPA_SUPPLICANT)	    += wpa_supplicant
dir_$(CONFIG_USER_802_1X)		    += 802.1x
dir_$(CONFIG_USER_DNSMASQ)                  += dnsmasq
dir_$(CONFIG_USER_DROPBEAR)                 += dropbear
dir_$(CONFIG_USER_IPROUTE2)                 += iproute2
dir_$(CONFIG_USER_PPPD)                	    += ppp
dir_$(CONFIG_USER_XL2TPD)		    += xl2tpd
dir_$(CONFIG_USER_PARPROUTED)		    += parprouted
dir_$(CONFIG_USER_IGMP_PROXY)               += igmpproxy
dir_$(CONFIG_USER_ETHTOOL)                  += ethtool
dir_$(CONFIG_USER_UDPXY)                    += udpxy
dir_$(CONFIG_USER_WIVE)			    += wive-utils
dir_$(CONFIG_USER_KABINET)		    += kabinet
dir_$(CONFIG_USER_MINIUPNPD)                += miniupnp
dir_$(CONFIG_USER_XUPNPD)		    += xupnpd
dir_$(CONFIG_USER_RADVD)               	    += radvd
dir_$(CONFIG_USER_DHCP6)                    += dhcp6
dir_$(CONFIG_USER_ZEBRA)		    += quagga
dir_$(CONFIG_USER_SNMPD)                    += snmpd
dir_$(CONFIG_USER_CHILLISPOT)               += chillispot
dir_$(CONFIG_USER_TCPDUMP)		    += tcpdump

#for usb support
dir_$(CONFIG_USB_MODESWITCH)                += usb_modeswitch
dir_$(CONFIG_USER_P910ND)		    += p910nd
dir_$(CONFIG_USER_NTFS3G)		    += ntfs-3g
dir_$(CONFIG_USER_SAMBA)		    += samba
dir_$(CONFIG_USER_SAMBA3)		    += samba3
dir_$(CONFIG_USER_TRANSMISSION)		    += transmission
dir_$(CONFIG_USER_STORAGE)		    += usb

#wifi simle config
dir_$(CONFIG_USER_WSC)                	    += wsc_upnp

#only ralink apps and scripts
dir_$(CONFIG_RT2880_APP)		    += rt2880_app

#need last busybox and goahead
dir_$(CONFIG_USER_BUSYBOX_BUSYBOX)          += busybox
dir_$(CONFIG_USER_GOAHEAD_HTTPD)	    += goahead

OPENSSL_CONFIG = RT2880 no-camellia no-capieng no-cms no-gmp no-jpake no-krb5 no-mdc2 no-rfc3779 no-seed \
                  no-ec no-ecdh no-ec2m no-ecdsa no-err no-threads no-idea no-rc2 no-rc5 \
                  no-hw no-md2 no-sse2 no-dso no-ts no-ripemd no-rmd160 no-sha0 \
                  no-smime no-ans1 no-cast no-whirlpool no-sctp no-srp no-jpake no-ssl2 no-ssl3 no-speed

all:
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
	# ">>>>>>>>>>>>>>>> CONFIGURE AND BUILD SSL <<<<<<<<<<<<<<<<<<<<<<<"
	if [ ! -e openssl/libssl.so ]; then  cd openssl ; ./Configure $(OPENSSL_CONFIG); $(MAKE) -j1 ; cd .. ; fi
	if [ -e openssl/libssl.so ]; then  cp -fap openssl/*.so* $(ROOTDIR)/lib/lib ; cp -fapv openssl/*.a $(ROOTDIR)/lib/lib ; fi
	if [ -e openssl/libssl.so ]; then  $(RM) -rf $(ROOTDIR)/lib/include_shared/openssl ; cp -faprL openssl/include/* $(ROOTDIR)/lib/include_shared ; fi
endif
	# ">>>>>>>>>>>>>>>> CONFIGURE ALL USERSPASE  <<<<<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -f "$$i/DoConfigure.sh" ]; then ( cd $$i; ./DoConfigure.sh || exit 1 ; cd .. ) || exit 1 ; fi \
	done
	# ">>>>>>>>>>>>>>>> MAKE USERSPACE  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
	for i in $(dir_y) ; do \
	    if [ -d "$$i" ]; then ( cd $$i; $(MAKE) -j$(HOST_NCPU) || exit 1 ; cd .. ) || exit 1 ; fi \
	done

romfs:
ifeq ($(CONFIG_USER_OPENSSL),y)
	if [ -e openssl/libssl.so ]; then  cd openssl ; $(MAKE) romfs ; cd .. ; fi
endif
	for i in $(sort $(dir_y)) ; do \
		[ ! -d $$i ] || $(MAKE) -C $$i romfs || exit $$? ; \
	done

clean:
	# ">>>>>>>>>>>>>>>> START FULL CLEANUP  <<<<<<<<<<<<<<<<<<<<<<"
	-cd openssl ; $(MAKE) clean ; $(MAKE) dclean ; cd ..
	-for i in $(SUBDIRS_APP); do $(MAKE) -C $$i clean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_APP); do $(MAKE) -C $$i distclean; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done
	-for i in $(SUBDIRS_APP); do $(MAKE) -C $$i mrproper; $(RM) -rf $(ROOTDIR)/user/$$i/filesystem/*; done

web:
	$(MAKE) -C goahead all
