#ifndef __UPLOAD_CGI_H__
#define __UPLOAD_CGI_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <sys/reboot.h>

/* for CONFIG_MTD_KERNEL_PART_SIZ  */
#include "linux/config.h"  				/* kernel config		*/
#include "config/autoconf.h"				/* user config			*/
#include "../../../tools/mkimage/include/image.h"	/* For Uboot image header format */

/* for calculate max image size */
#include "../../../linux/drivers/mtd/ralink/ralink-flash.h"
#define MAX_IMG_SIZE (IMAGE1_SIZE - MTD_RWFS_PART_SIZE - MTD_FACTORY_PART_SIZE - MTD_CONFIG_PART_SIZE - MTD_BOOT_PART_SIZE)

#include "../options.h"

inline unsigned int getMTDPartSize(char *part)
{
	char buf[128], name[32], size[32], dev[32], erase[32];
	unsigned int result=0;
	FILE *fp = fopen("/proc/mtd", "r");
	if(!fp){
		fprintf(stderr, "mtd support not enable?");
		return 0;
	}
	while(fgets(buf, sizeof(buf), fp)){
		sscanf(buf, "%s %s %s %s", dev, size, erase, name);
		if(!strcmp(name, part)){
			result = strtol(size, NULL, 16);
			break;
		}
	}
	fclose(fp);
	return result;
}

inline int mtd_write_firmware(char *filename, int offset, int len)
{
    char cmd[512];
    int status;
    int err=0;

/* check image size before erase flash and write image */
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
    if(len > MAX_IMG_SIZE ){
	fprintf(stderr, "Image in is BIG!!!%d", len);
        return -1;
    }
#endif

#if defined(CONFIG_RT2880_FLASH_8M) || defined(CONFIG_RT2880_FLASH_16M)
#ifdef CONFIG_RT2880_FLASH_TEST
    /* workaround: erase 8k sector by myself instead of mtd_erase */
    /* this is for bottom 8M NOR flash only */
    snprintf(cmd, sizeof(cmd), "/bin/flash -f 0x400000 -l 0x40ffff");
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#endif
#endif
#if defined(CONFIG_RT2880_ROOTFS_IN_RAM)
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel", offset, len, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#elif defined(CONFIG_RT2880_ROOTFS_IN_FLASH)
  #ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel_RootFS", offset, len, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
  #else
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel", offset,  CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;

    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s RootFS", offset + CONFIG_MTD_KERNEL_PART_SIZ, len - CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
  #endif
#else
    fprintf(stderr, "goahead: no CONFIG_RT2880_ROOTFS defined!");
#endif
    if (err == 0)
        return 0;
    else
        return -1;
}

inline void mtd_write_bootloader(char *filename, int offset, int len)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Bootloader", offset, len, filename);
	printf("write bootloader");
	system(cmd);
    return ;
}

#endif /* __UPLOAD_CGI_H__ */
