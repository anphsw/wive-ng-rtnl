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

#include "linux/autoconf.h"  //kernel config
#include "config/autoconf.h" //user config
					/* !!! for CONFIG_MTD_KERNEL_PART_SIZ  !!! */
                                        /*   CONFIG_RT2880_ROOTFS_IN_FLASH */
                                        /*   CONFIG_RT2880_ROOTFS_IN_RAM   */
#include "../options.h"

/*
 *  Uboot image header format
 *  (ripped from mkimage.c/image.h)
 */
typedef struct image_header {
    uint32_t    ih_magic;   /* Image Header Magic Number    */
    uint32_t    ih_hcrc;    /* Image Header CRC Checksum    */
    uint32_t    ih_time;    /* Image Creation Timestamp */
    uint32_t    ih_size;    /* Image Data Size      */
    uint32_t    ih_load;    /* Data  Load  Address      */
    uint32_t    ih_ep;      /* Entry Point Address      */
    uint32_t    ih_dcrc;    /* Image Data CRC Checksum  */
    uint8_t     ih_os;      /* Operating System     */
    uint8_t     ih_arch;    /* CPU architecture     */
    uint8_t     ih_type;    /* Image Type           */
    uint8_t     ih_comp;    /* Compression Type     */
    uint8_t     ih_name[IH_NMLEN];  /* Image Name       */
} image_header_t;

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

#if defined (CONFIG_RT2880_FLASH_8M) || defined (CONFIG_RT2880_FLASH_16M)
#ifdef CONFIG_RT2880_FLASH_TEST
    /* workaround: erase 8k sector by myself instead of mtd_erase */
    /* this is for bottom 8M NOR flash only */
    snprintf(cmd, sizeof(cmd), "/bin/flash -f 0x400000 -l 0x40ffff");
    system(cmd);
#endif
#endif
#if defined (CONFIG_RT2880_ROOTFS_IN_RAM)
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel", offset, len, filename);
    status = system(cmd);
#elif defined (CONFIG_RT2880_ROOTFS_IN_FLASH)
  #ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel_RootFS", offset, len, filename);
    status = system(cmd);
  #else
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel", offset,  CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = system(cmd);
    if(len > CONFIG_MTD_KERNEL_PART_SIZ ){
		snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s RootFS", offset + CONFIG_MTD_KERNEL_PART_SIZ, len - CONFIG_MTD_KERNEL_PART_SIZ, filename);
		status = system(cmd);
    }
  #endif
#else
    fprintf(stderr, "goahead: no CONFIG_RT2880_ROOTFS defined!");
#endif
    return 0;
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
