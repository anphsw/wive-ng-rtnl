/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <stdlib.h>             
#include <stdio.h>             
#include <string.h>           
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include "flash_ioctl.h"


int FlashRead(unsigned int *dst, unsigned int *src, unsigned int numBytes)
{
    struct flash_opt flash;
    int fd;

    if(numBytes > FLASH_MAX_RW_SIZE) {
	printf("Too many bytes - %d > %d bytes\n",numBytes, FLASH_MAX_RW_SIZE);
	return 0;
    }
    
    flash.dest=dst;
    flash.src=src;
    flash.bytes=numBytes;

    fd = open("/dev/flash0", O_RDONLY);
    if (fd < 0)
    {
	printf("Open flash pseudo device failed\n");
	return 0;
    }

    if(ioctl(fd, FLASH_IOCTL_READ, &flash)<0) {
	printf("FLASH_API: ioctl error\n");
	close(fd);
	return -1;
    }

    close(fd);
    if(flash.result==OUT_OF_SCOPE) {
	return -1;
    }else {
	return 0;
    }
}


int FlashWrite(unsigned int *source, unsigned int *dest, unsigned int numBytes)
{
    struct flash_opt flash;
    int fd;

    if(numBytes > FLASH_MAX_RW_SIZE) {
	printf("Too many bytes - %d > %d bytes\n",flash.bytes, FLASH_MAX_RW_SIZE);
	return 0;
    }
    
    flash.dest=dest;
    flash.src=source;
    flash.bytes=numBytes;

    fd = open("/dev/flash0", O_RDONLY);
    if (fd < 0)
    {
	printf("Open flash pseudo device failed\n");
	return 0;
    }

    if(ioctl(fd, FLASH_IOCTL_WRITE, &flash)<0) {
	printf("FLASH_API: ioctl error\n");
	close(fd);
	return -1;
    }

    close(fd);
    if(flash.result==OUT_OF_SCOPE) {
	return -1;
    }else {
	return 0;
    }
}

int FlashErase(unsigned int StartAddr, unsigned int EndAddr)
{
    struct flash_opt flash;
    int fd;

    flash.start_addr = StartAddr;
    flash.end_addr = EndAddr;

    fd = open("/dev/flash0", O_RDONLY);
    if (fd < 0)
    {
	printf("Open flash pseudo device failed\n");
	perror("open flash");	// tmp test
	return 0;
    }

    if(ioctl(fd, FLASH_IOCTL_ERASE, &flash)<0) {
	printf("FLASH_API: ioctl error\n");
	close(fd);
	return -1;
    }

    close(fd);
    if(flash.result==OUT_OF_SCOPE) {
	return -1;
    }else {
	return 0;
    }

}

