.EXPORT_ALL_VARIABLES:

DIRS = src cgi-src 
ROOT_DIRECTORY = /etc_ro/web

all:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

romfs:
	$(ROMFSINST) $(ROOT_DIRECTORY)
ifeq ("$(CONFIG_RT2860V2_STA)", "")
	rm -rf $(ROMFSDIR)/$(ROOT_DIRECTORY)/station
ifeq ("$(CONFIG_RT2860V2_AP_WSC)", "")
	rm -rf $(ROMFSDIR)/$(ROOT_DIRECTORY)/wps/wps_sta.asp
endif
endif
ifeq ("$(CONFIG_USER_STORAGE)", "")
	rm -rf $(ROMFSDIR)/$(ROOT_DIRECTORY)/storage
endif
ifneq ("$(CONFIG_RT2880v2_INIC)", "")
	$(ROMFSINST) inic $(ROOT_DIRECTORY)
endif
ifneq ("$(CONFIG_RT2561_AP)", "")
	$(ROMFSINST) legacy $(ROOT_DIRECTORY)
endif
ifneq ("$(CONFIG_USER_GOAHEAD_LANG_EN)", "y")
	rm -rf $(ROMFSDIR)/$(ROOT_DIRECTORY)/lang/en
endif
ifneq ("$(CONFIG_USER_GOAHEAD_LANG_ZHTW)", "y")
	rm -rf $(ROMFSDIR)/$(ROOT_DIRECTORY)/lang/zhtw
endif
ifneq ("$(CONFIG_USER_GOAHEAD_LANG_ZHCN)", "y")
	rm -rf $(ROMFSDIR)/$(ROOT_DIRECTORY)/lang/zhcn
endif
	for i in $(DIRS) ; do make -C $$i $@ ; done

clean:
	for i in $(DIRS) ; do make -C $$i clean ; done

