
-include config.arch
-include ../../../linux/drivers/mtd/ralink/ralink-flash.h
-include ../../../sdk_version.h
-include ../../../version

KERNELZ = $(IMAGEDIR)/zImage
RAMDISK  = $(IMAGEDIR)/ramdisk
IMAGE = $(IMAGEDIR)/$(PRODUCTID)_$(RT288X_SDK_VERSION).bin
ROMFS_DIR = $(ROOTDIR)/romfs

RAMDISK_SIZE  := $(shell echo $(CONFIG_BLK_DEV_RAM_SIZE))
MKIMAGE_DIR = $(ROOTDIR)/tools/mkimage
CUR_DIR = $(shell pwd)
COMP = lzma

ROMFS_DIRS = dev dev/pts sys proc tmp bin home mnt var etc web usr lib xml \
	lib/modules  usr/codepages \
	media \
	share

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
	$(ROMFSINST) -s ../etc/scripts/config.sh /bin/config.sh
	$(ROMFSINST) -s /proc/self/mounts /etc/mtab

romfs.post::
	[ -d $(IMAGEDIR) ] || mkdir -p $(IMAGEDIR)
	#Squash File System
	rm -f $(RAMDISK)
	#linux2.6.x support squashfs with lzma
ifeq	($(CONFIG_RT2880_ROOTFS_IN_FLASH),y)
	$(ROOTDIR)/tools/bin/mksquashfs_lzma-3.2 $(ROMFSDIR) $(RAMDISK) -le -all-root -no-exports
endif

mkimage: $(MKIMAGE_DIR)/crc32.c $(MKIMAGE_DIR)/mkimage.c 
	$(HOSTCC) $(HOST_CFLAGS) -DUSE_HOSTCC -o $@ -I$(MKIMAGE_DIR)/include $^ 

image: mkimage
	$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S $(ROOTDIR)/$(LINUXDIR)/vmlinux  $(KERNELZ)
	cd $(IMAGEDIR) ; rm -f $(KERNELZ).*; \
	$(ROOTDIR)/tools/bin/lzma_alone e -a1 -d$(CONFIG_LZMA_COMPRESS_LEVEL) $(KERNELZ) $(KERNELZ).$(COMP)

ifeq    ($(CONFIG_BLK_DEV_INITRD),)
ifeq    ($(CONFIG_ROOTFS_IN_FLASH_NO_PADDING),)
	# Original Kernel Image Size
	@wc -c $(KERNELZ).$(COMP)
ifeq    ($(findstring 0x, $(CONFIG_MTD_KERNEL_PART_SIZ)),0x)
	@SIZE=`wc -c $(KERNELZ).$(COMP) | awk '{ print $$1 }'` ; \
	MTD_KRN_PART_SIZE=`printf "%d" $(CONFIG_MTD_KERNEL_PART_SIZ)` ; \
	PAD=`expr $$MTD_KRN_PART_SIZE - 64 - $$SIZE` ; \
	dd if=/dev/zero count=1 bs=$$PAD conv=sync 2> /dev/null | tr \\000 \\377 >> $(KERNELZ).$(COMP)
else
	@SIZE=`wc -c $(KERNELZ).$(COMP) | awk '{ print $$1 }'` ; \
	MTD_KRN_PART_SIZE=`printf "%d" 0x$(CONFIG_MTD_KERNEL_PART_SIZ)` ; \
	PAD=`expr $$MTD_KRN_PART_SIZE - 64 - $$SIZE` ; \
	dd if=/dev/zero count=1 bs=$$PAD conv=sync 2> /dev/null | tr \\000 \\377 >> $(KERNELZ).$(COMP)
endif
	# Padded Kernel Image Size
	  @wc -c $(KERNELZ).$(COMP)
endif
	# Original RootFs Size
	  @du -sb $(ROMFSDIR)
	# Compressed RootFs Size
	  @wc -c $(RAMDISK)
	# Padded Kernel Image + Compressed Rootfs Size
	  @cat $(RAMDISK) >> $(KERNELZ).$(COMP)
	  @wc -c $(KERNELZ).$(COMP)
	#===========================================
endif
	# Pack final image and write headers
ifeq    ($(CONFIG_ROOTFS_IN_FLASH_NO_PADDING),)
	@cd $(IMAGEDIR) ; \
	$(CUR_DIR)/mkimage -A mips -O linux -T kernel -C $(COMP) -a 80000000 \
	-e $(shell readelf -h $(ROOTDIR)/$(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}') \
	-n $(PRODUCTID) -d $(KERNELZ).$(COMP) $(IMAGE)
else
	# For No padded need write kernel size in image header 
	# to correct mount particion in mtd drivers adress
	@cd $(IMAGEDIR) ; \
	ISIZE=`wc -c $(KERNELZ).$(COMP) | awk '{print $$1}'` ; \
	RSIZE=`wc -c $(RAMDISK) | awk '{print $$1}'` ; \
	KRN_SIZE=`expr $$ISIZE - $$RSIZE + 64` ; \
	$(CUR_DIR)/mkimage -A mips -O linux -T kernel -C $(COMP) -a 80000000 \
	-e $(shell readelf -h $(ROOTDIR)/$(LINUXDIR)/vmlinux | grep "Entry" | awk '{print $$4}') \
	-k $$KRN_SIZE -n $(PRODUCTID) -d $(KERNELZ).$(COMP) $(IMAGE)
endif
