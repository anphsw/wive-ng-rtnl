include config.arch
include ../../../linux-2.6.21.x/drivers/mtd/maps/ralink-flash.h

KERNELZ = $(IMAGEDIR)/zImage
RAMDISK  = $(IMAGEDIR)/ramdisk
IMAGE = $(IMAGEDIR)/$(PRODUCTID)_$(KERNEL_VER).$(FS_VER).trx
ROMFS_DIR = $(ROOTDIR)/romfs

RAMDISK_SIZE  := $(shell echo $(CONFIG_BLK_DEV_RAM_SIZE))
MKIMAGE_DIR = $(ROOTDIR)/tools/mkimage
CUR_DIR = $(shell pwd)
COMP = lzma

ROMFS_DIRS = \
	tmp \
	bin \
	dev dev/pts \
	home \
	etc \
	etc_ro etc_ro/web etc_ro/ppp etc_ro/ppp/peers etc_ro/ppp/3g \
	etc_ro/ppp/plugins \
	etc_ro/wlan etc_ro/Wireless \
	etc_ro/Wireless/RT2860AP etc_ro/xml \
	etc_ro/linuxigd \
	lib lib/modules lib/ipsec lib/pppd lib/l2tp lib/l2tp/plugins\
	mnt \
	proc \
	sys \
	media \
	share \
	usr usr/codepages \
	www \
	apps \
	var 

clean:
	rm -f mkimage

romfs: 
	[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		[ -d $(ROMFSDIR)/$$i ] || mkdir -p $(ROMFSDIR)/$$i; \
	done
	$(ROMFSINST) -s ./bin /sbin
	$(ROMFSINST) -s ../bin /usr/bin
	$(ROMFSINST) -s ../bin /usr/sbin
	$(ROMFSINST) -s ../lib /usr/lib
	$(ROMFSINST) -s ../share /usr/share
	$(ROMFSINST) /etc_ro/Wireless/RT2860AP/RT2860_default_vlan
	$(ROMFSINST) /etc_ro/Wireless/RT2860AP/RT2860_default_novlan
	$(ROMFSINST) /etc_ro/Wireless/RT2860AP/RT2860_default_oneport
	$(ROMFSINST) -e CONFIG_RALINK_RT3052AP_2T2R /etc_ro/wlan/RT3052_AP_2T2R_V1_1.bin
	$(ROMFSINST) -e CONFIG_RALINK_RT3051AP_1T2R /etc_ro/wlan/RT3051_AP_1T2R_V1_0.bin
	$(ROMFSINST) -e CONFIG_RALINK_RT3050AP_1T1R /etc_ro/wlan/RT3050_AP_1T1R_V1_0.bin

romfs.post:: 
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	#Squash File System
	 rm -f $(RAMDISK)  
	#linux2.6.x support squashfs with lzma
ifeq	($(CONFIG_BLK_DEV_INITRD),)
	$(ROOTDIR)/tools/bin/mksquashfs_lzma-3.2 $(ROMFSDIR) $(RAMDISK)
endif

mkimage: $(MKIMAGE_DIR)/crc32.c $(MKIMAGE_DIR)/mkimage.c 
	$(HOSTCC) $(HOST_CFLAGS) -DUSE_HOSTCC -o $@ -I$(MKIMAGE_DIR)/include $^ 

image: mkimage
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(ROOTDIR)/$(LINUXDIR)/vmlinux  $(KERNELZ)
	cd $(IMAGEDIR) ; rm -f $(KERNELZ).*; \
	$(ROOTDIR)/tools/bin/lzma_alone e -a1 -d$(CONFIG_LZMA_COMPRESS_LEVEL) $(KERNELZ) $(KERNELZ).$(COMP)

ifeq ($(CONFIG_BLK_DEV_INITRD),)
ifeq ($(CONFIG_ROOTFS_IN_FLASH_NO_PADDING),)
	# Original Kernel Image Size
	@wc -c $(KERNELZ).$(COMP)
	# Padded Kernel Image Size
ifeq ($(findstring 0x, $(CONFIG_MTD_KERNEL_PART_SIZ)),0x)
	@SIZE=`wc -c $(KERNELZ).$(COMP) | awk '{ print $$1 }'` ; \
	MTD_KRN_PART_SIZE=`printf "%d" $(CONFIG_MTD_KERNEL_PART_SIZ)` ; \
	PAD=`expr $$MTD_KRN_PART_SIZE - 64 - $$SIZE` ; \
	dd if=/dev/zero count=1 bs=$$PAD 2> /dev/null | tr \\000 \\377 >> $(KERNELZ).$(COMP)
else
	@SIZE=`wc -c $(KERNELZ).$(COMP) | awk '{ print $$1 }'` ; \
	MTD_KRN_PART_SIZE=`printf "%d" 0x$(CONFIG_MTD_KERNEL_PART_SIZ)` ; \
	PAD=`expr $$MTD_KRN_PART_SIZE - 64 - $$SIZE` ; \
	dd if=/dev/zero count=1 bs=$$PAD 2> /dev/null | tr \\000 \\377 >> $(KERNELZ).$(COMP)
endif
endif
	@wc -c $(KERNELZ).$(COMP)
	# Original RootFs Size
	@du -sb $(ROMFSDIR)
	# Compressed RootFs Size
	@wc -c $(RAMDISK)
	# Padded Kernel Image + Compressed Rootfs Size
	@cat $(RAMDISK) >> $(KERNELZ).$(COMP)
	@wc -c $(KERNELZ).$(COMP)
	#===========================================
endif

ifeq ($(CONFIG_ROOTFS_IN_FLASH_NO_PADDING),y)
	@cd $(IMAGEDIR) ; \
	ISIZE=`wc -c $(KERNELZ).$(COMP) | awk '{print $$1}'` ; \
	RSIZE=`wc -c $(RAMDISK) | awk '{print $$1}'` ; \
	KRN_SIZE=`expr $$ISIZE - $$RSIZE + 64` ; \
	ENTRY=`readelf -h $(ROOTDIR)/$(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}'` ; \
	$(CUR_DIR)/mkimage -A mips -O linux -T kernel -C $(COMP) -a 80000000 -e $$ENTRY -k $$KRN_SIZE \
	-n $(PRODUCTID) -d $(KERNELZ).$(COMP) $(IMAGE)
else
	@cd $(IMAGEDIR) ;
	ENTRY=`readelf -h $(ROOTDIR)/$(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}'` ; \
	$(CUR_DIR)/mkimage -A mips -O linux -T kernel -C $(COMP) -a 80000000 \
	-e $$ENTRY -n $(PRODUCTID) -d $(KERNELZ).$(COMP) $(IMAGE)
endif
