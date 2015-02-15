###########################################################################
#
# Makefile -- Top level uClinux makefile.
#
# Copyright (c) 2001-2004, SnapGear (www.snapgear.com)
# Copyright (c) 2001, Lineo
#

############################################################################
#
# Lets work out what the user wants, and if they have configured us yet
#

.EXPORT_ALL_VARIABLES:

ROOTDIR	:= $(shell pwd)

ifeq (.config,$(wildcard .config))
-include version
-include .config
all: linux tools lib_only user_only romfs image
else
all: config_error
endif

VERSIONSTR = $(CONFIG_VENDOR)/$(CONFIG_PRODUCT) Version $(VERSIONPKG)

#########################################################################################################
# Tune locale to eng
#
LANG		:= C
LC_COLLATE	:= C
LC_MESSAGES	:= C
LC_ALL		:= C

#########################################################################################################
# Cleanup CFLAGS/CPPFLAGS
#
CFLAGS		:=
CPPFLAGS	:=
CXXFLAGS	:=

############################################################################
#
# Get the core stuff worked out
#

MAKE		?= make
HOSTCC		?= gcc

ROMFSINST	:= romfs-inst.sh
TFTPDIR		:= /tftpboot

LINUXDIR	:= $(CONFIG_LINUXDIR)
LIBCDIR		:= $(CONFIG_LIBCDIR)
IMAGEDIR	:= $(ROOTDIR)/images
ROMFSDIR	:= $(ROOTDIR)/romfs
SCRIPTSDIR	:= $(ROOTDIR)/config/scripts
LINUX_CONFIG	:= $(ROOTDIR)/$(LINUXDIR)/.config
CONFIG_CONFIG	:= $(ROOTDIR)/config/.config
PATH		:= $(PATH):$(ROOTDIR):$(ROOTDIR)/tools:$(ROOTDIR)/toolchain/bin:$(ROOTDIR)/lib/lib:$(ROOTDIR)/lib/include:$(LINUXDIR):$(LIBCDIR)

# May use a different compiler
CROSS_COMPILE		?= $(ROOTDIR)/toolchain/bin/mipsel-linux-uclibc-
KERNEL_CROSS_COMPILE	:= $(CROSS_COMPILE)
CROSS_COMPILER_PREFIX	:= $(CROSS_COMPILE)

#NUM MAKE PROCESS = CPU NUMBER IN THE SYSTEM * CPU_OVERLOAD
CPU_OVERLOAD	:= 4

HOST_NCPU	:= $(shell if [ -f /proc/cpuinfo ]; then n=`grep -c processor /proc/cpuinfo`; if [ $$n -gt 1 ];then expr $$n \* ${CPU_OVERLOAD}; else echo $$n; fi; else echo 1; fi)
BUILD_START_STRING := $(shell date "+%a, %d %b %Y %T %z")

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; else if [ -x /bin/bash ]; then echo /bin/bash; else echo sh; fi ; fi)

ifeq (config.arch,$(wildcard config.arch))
ifeq ($(filter %_default, $(MAKECMDGOALS)),)
ARCH_CONFIG := $(ROOTDIR)/config.arch
include $(ARCH_CONFIG)
export ARCH_CONFIG
endif
endif

ifneq ($(SUBARCH),)
# Using UML, so make the kernel and non-kernel with different ARCHs
MAKEARCH := $(MAKE) ARCH=$(SUBARCH) CROSS_COMPILE=$(CROSS_COMPILE)
MAKEARCH_KERNEL := $(MAKE) ARCH=$(ARCH) SUBARCH=$(SUBARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)
else
MAKEARCH := $(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)
MAKEARCH_KERNEL := $(MAKEARCH)  ARCH=$(ARCH) CROSS_COMPILE=$(KERNEL_CROSS_COMPILE)
endif

DIRS    :=  $(ROOTDIR)/vendors $(ROOTDIR)/user $(ROOTDIR)/lib

export LANG LC_COLLATE LC_MESSAGES LC_ALL
export VENDOR PRODUCT ROOTDIR LINUXDIR HOSTCC CONFIG_SHELL
export CONFIG_CONFIG LINUX_CONFIG ROMFSDIR SCRIPTSDIR
export RT288X_SDK_VERSION VERSIONPKG VERSIONSTR ROMFSINST PATH IMAGEDIR RELFILES TFTPDIR
export BUILD_START_STRING
export HOST_NCPU DIRS

############################################################################

#
# Config stuff,  we recall ourselves to load the new config.arch before
# running the kernel and other config scripts
#

.PHONY: config.tk config.in

config.in:
	@chmod u+x config/mkconfig
	config/mkconfig > config.in

config.tk: config.in
	$(MAKE) -C $(SCRIPTSDIR) tkparse
	ARCH=dummy $(SCRIPTSDIR)/tkparse < config.in > config.tmp
	@if [ -f /usr/local/bin/wish ];	then \
		echo '#!'"/usr/local/bin/wish -f" > config.tk; \
	else \
		echo '#!'"/usr/bin/wish -f" > config.tk; \
	fi
	cat $(SCRIPTSDIR)/header.tk >> ./config.tk
	cat config.tmp >> config.tk
	rm -f config.tmp
	echo "set defaults \"/dev/null\"" >> config.tk
	echo "set help_file \"config/Configure.help\"" >> config.tk
	cat $(SCRIPTSDIR)/tail.tk >> config.tk
	chmod 755 config.tk

.PHONY: config
config: config.in
	@HELP_FILE=config/Configure.help \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Configure config.in
	@chmod u+x config/setconfig
	@config/setconfig defaults
	@if egrep "^CONFIG_DEFAULTS_KERNEL=y" .config > /dev/null; then \
		$(MAKE) linux_config; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_VENDOR=y" .config > /dev/null; then \
		$(MAKE) config_config; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_BUSYBOX=y" .config > /dev/null; then \
		$(MAKE) -C user/busybox menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC=y" .config > /dev/null; then \
		$(MAKE) -C lib menuconfig; \
	 fi
	@config/setconfig final

.PHONY: menuconfig
menuconfig: config.in
	$(MAKE) -C $(SCRIPTSDIR)/lxdialog all
	@HELP_FILE=config/Configure.help \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Menuconfig config.in
	@if [ ! -f .config ]; then \
		echo; \
		echo "You have not saved your config, please re-run make config"; \
		echo; \
		exit 1; \
	 fi
	@chmod u+x config/setconfig
	@config/setconfig defaults
	@if egrep "^CONFIG_DEFAULTS_KERNEL=y" .config > /dev/null; then \
		$(MAKE) linux_menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_VENDOR=y" .config > /dev/null; then \
		$(MAKE) config_menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_BUSYBOX=y" .config > /dev/null; then \
		$(MAKE) -C user/busybox menuconfig; \
	 fi
	@if egrep "^CONFIG_DEFAULTS_UCLIBC=y" .config > /dev/null; then \
		$(MAKE) -C lib menuconfig; \
	 fi
	@config/setconfig final

.PHONY: oldconfig
oldconfig: config.in
	@HELP_FILE=config/Configure.help $(CONFIG_SHELL) $(SCRIPTSDIR)/Configure -d config.in
	@$(MAKE) oldconfig_linux
	@$(MAKE) oldconfig_config
	@chmod u+x config/setconfig
	@config/setconfig final

linux_menuconfig:
	KCONFIG_NOTIMESTAMP=1 $(MAKEARCH_KERNEL) -C $(LINUXDIR) menuconfig
linux_config:
	KCONFIG_NOTIMESTAMP=1 $(MAKEARCH_KERNEL) -C $(LINUXDIR) config
config_xconfig:
	$(MAKEARCH) -C config xconfig
config_menuconfig:
	$(MAKEARCH) -C config menuconfig
config_config:
	$(MAKEARCH) -C config config
oldconfig_config:
	$(MAKEARCH) -C config oldconfig
oldconfig_linux:
	KCONFIG_NOTIMESTAMP=1 $(MAKEARCH_KERNEL) -C $(LINUXDIR) oldconfig

############################################################################
#
# normal make targets
#

.PHONY: romfs
romfs: romfs.subdirs modules_install romfs.post

.PHONY: romfs.subdirs
romfs.subdirs:
	####################PREPARE-ROMFS####################
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	$(MAKEARCH) -C vendors romfs
	cd $(ROOTDIR)
	cp -vfra $(ROOTDIR)/etc/* $(ROMFSDIR)/etc
	cp -vfa  $(ROOTDIR)/etc/rc.d/rcS $(ROMFSDIR)/bin/rcS
	cp -vfa  $(ROOTDIR)/etc/rc.d/start $(ROMFSDIR)/bin/start
	fakeroot tar -zxvf dev.tgz
	fakeroot cp -rfva dev/* $(ROMFSDIR)/dev
	cp $(ROOTDIR)/version $(ROMFSDIR)/etc/version
	cd $(ROMFSDIR)/bin && /bin/ln -fvs ../etc/scripts/* . && cd $(ROOTDIR)
	cd $(ROOTDIR)
	#################INSTALL-APPS-ROMFS###################
	for dir in $(DIRS) ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir romfs || exit 1 ; done

.PHONY: modules_install
modules_install:
	#----------------------------INSTALL_MODULES----------------------------------
	. $(LINUXDIR)/.config; if [ "$$CONFIG_MODULES" = "y" ]; then \
		[ -d $(ROMFSDIR)/lib/modules ] || mkdir -p $(ROMFSDIR)/lib/modules; \
		$(MAKEARCH_KERNEL) -C $(LINUXDIR) INSTALL_MOD_PATH=$(ROMFSDIR) DEPMOD="../user/busybox/examples/depmod.pl" modules_install; \
	fi

.PHONY: romfs.post
romfs.post:
	##################COPY_RUNTIME_LIBS####################
	find $(ROOTDIR)/toolchain/lib -type f -name 'libgcc_s*so*' -exec cp -vfap {} $(ROMFSDIR)/lib/ \;
	######################CLEANUP##########################
	-find $(ROMFSDIR)/. -name CVS | xargs -r rm -rf
	-rm -f $(ROMFSDIR)/lib/modules/*/build
	-rm -f $(ROMFSDIR)/lib/modules/*/source
	-rm -fr $(ROOTDIR)/dev
	#################STRIP_APPS_LIB_ROMFS##################
	./strip.sh
	###################SET_COMPILE_DATE####################
	date +%Y%m%d%H%M > $(ROMFSDIR)/etc/compile-date
	###################APPS-INSTALLED######################
	$(MAKEARCH) -C vendors romfs.post

.PHONY: image
image:
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	$(MAKEARCH) -C vendors image

.PHONY: release
release:
	$(MAKE) -C release release

#
# fancy target that allows a vendor to have other top level
# make targets,  for example "make vendor_flash" will run the
# vendor_flash target in the vendors directory
#
vendor_%:
	$(MAKEARCH) -C vendors $@

# for select build mode (one/multi thread)
-include $(LINUX_CONFIG)
ifeq ($(CONFIG_RT2860V2_STA),m)
# Added by Steven@Ralink FIXME!!!
# In linux-2.6, it do not support VPATH in Makefile.
# But we need to use drivers/net/wireless/rt2860v2 to build ap and sta driver.
# Workaround: Don't build ap and sta driver at the same time.
THREADS="-j1"
else
THREADS="-j$(HOST_NCPU)"
endif

.PHONY: linux
linux:
	$(MAKEARCH_KERNEL) $(THREADS) -C $(LINUXDIR) $(LINUXTARGET) || exit 1
	if [ -f $(LINUXDIR)/vmlinux ]; then \
		ln -f $(LINUXDIR)/vmlinux $(LINUXDIR)/linux ; \
	fi

.PHONY: sparse
sparse:
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) C=1 $(LINUXTARGET) || exit 1

.PHONY: sparseall
sparseall:
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) C=2 $(LINUXTARGET) || exit 1

.PHONY: subdirs
subdirs: lib linux
	for dir in $(DIRS) ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir || exit 1 ; done

dep:
	@if [ ! -f $(LINUXDIR)/.config ] ; then \
		echo "ERROR: you need to do a 'make config' first" ; \
		exit 1 ; \
	fi
	@if [ $(LINUXDIR) = linux ] ; then \
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) prepare ; \
	fi
	$(MAKEARCH_KERNEL) -C $(LINUXDIR) dep

.PHONY: tools
tools:
	$(MAKE) -C tools

# This one removes all executables from the tree and forces their relinking
.PHONY: relink
relink:
	find user prop vendors -type f -name '*.gdb' | sed 's/^\(.*\)\.gdb/\1 \1.gdb/' | xargs rm -f

clean:
	################PREPARE FOR CLEANUP############################
	touch $(ROOTDIR)/lib/.config
	touch $(ROOTDIR)/linux/.config
	touch $(ROOTDIR)/.config
	touch $(ROOTDIR)/config/autoconf.h
	touch $(ROOTDIR)/config.arch
	################CLEAN ALL SUBDIRS 1############################
	-for dir in $(DIRS) $(LINUXDIR) Uboot ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir clean ; done
	-for dir in $(DIRS) $(LINUXDIR) Uboot ; do [ ! -d $$dir ] || $(MAKEARCH) -C $$dir distclean ; done
	################CLEAN ALL SUBDIRS 2############################
	-$(MAKE) clean -C config
	-$(MAKE) clean -C tools
	-$(MAKE) clean -C libnvram
	-$(MAKE) clean -C libext
	-$(MAKE) mrproper -C Uboot
	-$(MAKE) mrproper -C linux
	##############REMOVE UNUSED FILES 1###########################
	rm -f $(ROOTDIR)/etc/compile-date $(ROOTDIR)/etc/scripts/config.sh $(ROOTDIR)/config.tk $(ROOTDIR)/.tmp* $(ROOTDIR)/sdk_version.h $(ROOTDIR)/version $(LINUXDIR)/linux $(LINUXDIR)/arch/mips/ramdisk/*.gz
	##############REMOVE UNUSED FILES 2###########################
	find $(ROOTDIR) -type f -a \( -name '*.*~' -o -name '*.ko' -o -name '*.log' -o -name '*.old' -o -name 'config.log' -o -name 'aclocal.m4' -o -name '.sgbuilt_user' -o -name '.config.cmd' \) | xargs rm -f
	##############REMOVE UNUSED FILES 3###########################
	find $(ROOTDIR)/lib $(ROOTDIR)/libext $(ROOTDIR)/user -type f -a \( -name '*.o' -o -name '*.a' -o -name '*.so' -o -name '*.lo' -o -name '*.la' \) | xargs rm -f 
	##############REMOVE UNUSED FOLDERS###########################
	find $(ROOTDIR) -type d -a \( -name 'filesystem' -o -name 'autom4te.cache' -o -name '.libs' -o -name 'cvs' -o -name 'CVS' -o -name '.dep' -o -name '.deps' \) | xargs rm -rf
	rm -rf $(ROOTDIR)/dev $(IMAGEDIR) $(ROMFSDIR) romfs config.in config.arch config.tk images modules/config.tk .config .config.old .oldconfig autoconf.h

%_only:
	@case "$(@)" in \
	*/*) d=`expr $(@) : '\([^/]*\)/.*'`; \
	     t=`expr $(@) : '[^/]*/\(.*\)'`; \
	     $(MAKEARCH) -j$(HOST_NCPU) -C $$d $$t;; \
	*)   $(MAKEARCH) -j$(HOST_NCPU) -C $(@:_only=);; \
	esac

%_clean:
	@case "$(@)" in \
	*/*) d=`expr $(@) : '\([^/]*\)/.*'`; \
	     t=`expr $(@) : '[^/]*/\(.*\)'`; \
	     $(MAKEARCH) -C $$d $$t;; \
	*)   $(MAKEARCH) -C $(@:_clean=) clean;; \
	esac

%_default:
	@if [ ! -f "vendors/$(@:_default=)/config.device" ]; then \
		echo "vendors/$(@:_default=)/config.device must exist first"; \
		exit 1; \
	 fi
	-$(MAKE) clean > /dev/null 2>&1
	cp vendors/$(@:_default=)/config.device .config
	$(MAKE) dep
	$(MAKE)

dist-prep:
	-find $(ROOTDIR) -name 'Makefile*.bin' | while read t; do \
		$(MAKEARCH) -C `dirname $$t` -f `basename $$t` $@; \
	 done

web: web_make romfs image

web_make:
	$(MAKE) -C user web
