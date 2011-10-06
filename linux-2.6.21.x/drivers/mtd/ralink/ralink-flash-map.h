/* Include defines */
#include "ralink-flash.h"

/* For autodetect flash type */
#if defined (CONFIG_RALINK_RT2880) || \
    defined (CONFIG_RALINK_RT2883) || \
    defined (CONFIG_RALINK_RT3883) || \
    defined (CONFIG_RALINK_RT3352) || \
    defined (CONFIG_RALINK_RT3052) || \
    defined (CONFIG_RALINK_RT5350)

    #define BOOT_FROM_NOR   0
    #define BOOT_FROM_NAND  2
    #define BOOT_FROM_SPI   3
    extern int ra_check_flash_type(void);
#endif

/* this is mapping flash definition */
#if defined (CONFIG_RT2880_FLASH_32M)
static struct mtd_partition rt2880_partitions[] __maybe_unused = {
        {
                name:           "Bootloader",  /* mtdblock0 */
                size:           MTD_BOOT_PART_SIZE,  /* 192K */
                offset:         0,
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
        }, {
                name:           "Kernel",
                size:           CONFIG_MTD_KERNEL_PART_SIZ,
                offset:         MTDPART_OFS_APPEND,
        }, {
                name:           "RootFS",
                size:           MTD_ROOTFS_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#else //CONFIG_RT2880_ROOTFS_IN_RAM
        }, {
                name:           "Kernel",
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#endif
        }, {
                name:           "Config",
                size:           0x20000,  /* 128K */
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Factory",
                size:           0x20000,  /* 128K */
                offset:         MTDPART_OFS_APPEND
	}, {
                name:           "RW-FS", /* mtdblock4 */
                size:           MTD_RWFS_PART_SIZE, /* 128K */
                offset:         MTD_RWFS_PART_OFFSET,
#ifdef CONFIG_DUAL_IMAGE
	}, {
		name:		"Kernel2", /* mtdblock5 */
		size:		MTD_KERN2_PART_SIZE, 
		offset:		MTD_KERN2_PART_OFFSET,
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
	}, {
                name:           "RootFS2",
                size:           MTD_ROOTFS2_PART_SIZE,
                offset:         MTD_KERN2_PART_OFFSET,
#endif
#endif
	}
};
#else /* not 32M flash */
static struct mtd_partition rt2880_partitions[] __maybe_unused = {
        {
                name:           "Bootloader",	/* mtdblock0 */
                size:           MTD_BOOT_PART_SIZE,	/* 192K */
                offset:         0,
        }, {
                name:           "Config",	/* mtdblock1 */
                size:           MTD_CONFIG_PART_SIZE,	/* 64K */
                offset:         MTDPART_OFS_APPEND
        }, {
                name:           "Factory",	/* mtdblock2 */
                size:           MTD_FACTORY_PART_SIZE,	/* 64K */
                offset:         MTDPART_OFS_APPEND
#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH) && defined(CONFIG_ROOTFS_IN_FLASH_NO_PADDING) 
        }, {
                name:           "Kernel_stub", /* mtdblock3 */
                size:           0,
		offset:         MTDPART_OFS_APPEND,
        }, {
                name:           "RootFS_stub", /* mtdblock3 */
                size:           MTD_ROOTFS_PART_SIZE,
		offset:         MTDPART_OFS_APPEND,
#else
        }, {
                name:           "Kernel", 	/* mtdblock3 */
                size:           MTD_KERN_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#if  defined(CONFIG_RT2880_ROOTFS_IN_FLASH) && ! defined(CONFIG_ROOTFS_IN_FLASH_NO_PADDING)
        }, {
                name:           "RootFS", 	/* mtdblock4 */
                size:           MTD_ROOTFS_PART_SIZE,
                offset:         MTDPART_OFS_APPEND,
#endif
#endif
	}, {
                name:           "RW-FS",	/* mtdblock5 */ 
                size:           MTD_RWFS_PART_SIZE,	/* 128K */
                offset:         MTD_RWFS_PART_OFFSET,
#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH) && defined(CONFIG_ROOTFS_IN_FLASH_NO_PADDING) 
        }, {	// Only for update firmware in not padded mode
                name:           "Kernel_RootFS", /* mtdblock6 */
                size:           UPD_RWFS_PART_SIZE,
		offset:         UPD_RWFS_PART_OFFSET,
#endif
#ifdef CONFIG_DUAL_IMAGE
	}, {
		name:		"Kernel2", /* mtdblock6/7 */
		size:		MTD_KERN2_PART_SIZE, 
		offset:		MTD_KERN2_PART_OFFSET,
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
	}, {
                name:           "RootFS2", /* mtdblock7/8 */
                size:           MTD_ROOTFS2_PART_SIZE,
                offset:         MTD_ROOTFS2_PART_OFFSET,
#endif
#endif 
	}
};
#endif
