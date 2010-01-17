#
#	Makefile -- Build instructions for user level apps
#

.EXPORT_ALL_VARIABLES:
.PHONY: config all romfs clean prune

#
# Include architecture specific build rules.
#
ifndef ROOTDIR
ROOTDIR=../..
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

dir_$(CONFIG_ASUS_SHARED)                   += libdisk
dir_$(CONFIG_ASUS_NETCONF)                  += netconf
dir_$(CONFIG_ASUS_NVRAM)		    += nvram
dir_$(CONFIG_USER_MTDUTILS)                 += mtd-utils
dir_$(CONFIG_ASUS_NETCONF)		    += detectWAN
dir_$(CONFIG_ASUS_INFOSVR)                  += infosvr
dir_$(CONFIG_EZ_IPUPDATE)                   += ez-ipupdate
dir_$(CONFIG_ASUS_WWW)                      += www
dir_$(CONFIG_ASUS_HTTPD)                    += httpd
dir_$(CONFIG_ASUS_RC)                       += rc
dir_$(CONFIG_RT2880_NETWORK_MAP) += network_map

dir_$(CONFIG_USER_WSC)			    += wsc_upnp
dir_$(CONFIG_USER_UPNP_IGD)                 += linux-igd
#dir_$(CONFIG_USER_MINIUPNPD)          	    += upnp

all: config
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

