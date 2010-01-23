RELEASEDIR := $(shell pwd)
PATH := $(CROSS_DEV)/tools:$(CROSS_DEV)/cross-dev/usr/bin:$(PATH)
INSTALLDIR := $(RELEASEDIR)/rootfs/rootdir
CROSS_COMPILE :=mips_4KCle-
CROSS_COMPILER_PREFIX :=mips_4KCle-
# use uClibc to build the user space program
CLIB :=uClibc  
# use glibc to build the user space program
#CLIB :=glibc
VMLINUX := src/linux/vmlinux
DEFCONFIG := src/linux/arch/mips/rt2880/defconfig-rt2880
export CROSS_COMPILE CROSS_COMPILER_PREFIX CLIB
all:  mklink build_root image rootfs

mklink:
	rm -rf image/*
	rm -rf rootfs/root.cramfs
	rm -f src/linux;\
	cd src;\
	ln -sf linux-2.4.30 linux; \
	cd $(RELEASEDIR);\
	rm -f src/uboot ;\
	cd src;\
	ln -sf u-boot-1.1.3 uboot; \
	cd $(RELEASEDIR);\
	cd src/uboot;\
	make rt2880_config;\
	cd $(RELEASEDIR);

## added by bobtseng ++, 2005.12.21.
raether:
	rm -rf src/raether/*.o src/raether/mod/*
	rm -rf rootfs/rootdir/lib/modules/2.4.30/*
	$(MAKE) -C src/raether CROSS=$(CROSS_COMPILE) TARGETDIR=
	cp -f src/raether/raeth.o rootfs/rootdir/lib/modules/2.4.30
	cp -f src/raether/test.o rootfs/rootdir/lib/modules/2.4.30

realtek:
	rm -rf src/raether/*.o
	$(MAKE) -C src/raether rtl CROSS=mips-linux- TARGETDIR=
	cp src/raether/raeth.o /tftpboot

## added by bobtseng --

xconfig:
	make -C src/linux xconfig
modules:
	make -C src/linux modules
busybox:
	make -C src/router/busybox-1.00 xconfig
dep:
	make -C src/linux symlinks
	make -C src/linux dep
	cd src/uboot; rm -f `find . -name .depend`;
	cd src/router/busybox-1.00; rm -f .depend

build_root:
	if [ -f $(INSTALLDIR)/root.tgz ] ;\
	then cd $(INSTALLDIR);  sudo tar xvfz root.tgz ; sudo chown -R $(USER):$(USER) * ; rm -rf root.tgz;  fi;

boot_code:
#	make -C src/surfmilo/mips/build_rom #for surfmilo build
#	make -C src/rtl_boot/mips/build_rom #for rtl_boot build
	make -C src/uboot
	cp src/uboot/u-boot.bin /tftpboot/$(USER)_uboot.bin
	cd /tftpboot; mkimage -A mips -O u-boot -T firmware -C none -n "uBoot" -d $(USER)_uboot.bin $(USER)_uBoot


clean:mklink 
	rm -rf image/*
	rm -rf rootfs/root.cramfs
	rm -rf rootfs/rootdir/lib/modules/2.4.30/*
	make -C src/linux clean
	make -C src/uboot clean
	make -C src/uClibc-0.9.27 clean
	make -C src/router clean

router:
	echo "Make router code here"
	make -C src/router $(MFLAGS) INSTDIR=$(INSTALLDIR) TOPDIR=$(RELEASEDIR)


image: $(RELEASEDIR)/image/vmlinux
	cp $^ /tftpboot/$(USER)_vmlinux ; chmod 777 /tftpboot/$(USER)_vmlinux
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S /tftpboot/$(USER)_vmlinux  /tftpboot/$(USER)_vmlinux.bin
	cd /tftpboot ; rm -f $(USER)_vmlinux.bin.gz; gzip -9 $(USER)_vmlinux.bin
	cd /tftpboot ; mkimage -A mips -O linux -T kernel -C gzip -a 8a000000 -e $(shell readelf -h $^ | grep "Entry" | awk '{print $$4}') -n "Linux Kernel Image"  -d $(USER)_vmlinux.bin.gz $(USER)_uImage
	cp /tftpboot/$(USER)_uImage image

kernel:
	cd image;rm -f image/vmlinux
	$(shell [ ! -f src/linux/.config ] && cp $(DEFCONFIG) src/linux/.config)
	$(MAKE) -C src/linux 
	cd image;rm -f vmlinux.bin.gz
	cp -f src/linux/vmlinux image
	cd image;$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S vmlinux  vmlinux.bin ;gzip -9 vmlinux.bin
	cd image ; sync ;sync; mkimage -A mips -O linux -T kernel -C gzip -a 8a000000 -e $(shell readelf -h $(RELEASEDIR)/image/vmlinux | grep "Entry" | awk '{print $$4}') -n "Linux Kernel Image"  -d vmlinux.bin.gz uImage ; rm -f vmlinux.bin.gz

#rootfs: raether build_root boot_code uClibc mk_cramfs
rootfs: raether uClibc mk_cramfs
	cd /tftpboot ; mkimage -A mips -T multi -C none -n Multi_Image -d $(USER)_uImage:$(USER)_uRoot $(USER)_image

uClibc:
	$(MAKE) -C src/uClibc-0.9.27 CROSS=$(CROSS_COMPILE) ROOTDIR=$(RELEASEDIR)

mk_cramfs:
	rm -rf $(INSTALLDIR)/CVS
	cd rootfs && mkcramfs rootdir root.cramfs ;cp root.cramfs ../image; cp root.cramfs /tftpboot/$(USER)_root.cramfs
	cd /tftpboot; mkimage -A mips -T ramdisk -C none -n "CramFS" -d $(USER)_root.cramfs $(USER)_uRoot

$(RELEASEDIR)/image/vmlinux: kernel
	install -d $(RELEASEDIR)/image
	cp $(VMLINUX) $(RELEASEDIR)/image

.PHONY: all image
