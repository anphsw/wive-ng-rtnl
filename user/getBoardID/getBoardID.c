/*
 * getBoardID.c - read DUT board ID
 * Tom.Hung 2010-5-17
 * 
 * RT2880/RT3052:
 * 	BoardID = 0xD0~D2 (400d0~400d2)
 * 
 * RT3883:
 * 	BoardID = 0x01D0~0x01D2 (401d0~401d2)
 *
 * patch log:
 * 2009-4-29, revise to read boardID from flash directly (original is read from ra0)
 * 2009-5-17, change flash_read function for SDK_3.4.0.0, define BoardID address for RT3883
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <linux/autoconf.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "nvram.h"
#include "flash_ioctl.h"

#define NUM_INFO 6 //count of mtdparts
//define DEBUG

struct mtd_info {
	char dev[8];
	int size;
	int erasesize;
	char name[12];
} info[NUM_INFO];

int total_sz;

int init_info(void)
{
	FILE *fp;
	char line[128];
	int i=0, sz, esz, nm[12];

	memset(info, 0, sizeof(info));
	if ((fp = fopen("/proc/mtd", "r"))) {
		fgets(line, sizeof(line), fp); //skip the 1st line
		while (fgets(line, sizeof(line), fp)) {
			if (sscanf(line, "mtd%d: %x %x \"%s\"", &i, &sz, &esz,
						nm)) {
				if (i >= NUM_INFO)
					printf("please enlarge 'NUM_INFO'\n");
				else {
					sprintf(info[i].dev, "mtd%d", i);
					info[i].size = sz;
					info[i].erasesize = esz;
					nm[strlen((char *)nm)-2] = '\0'; //FIXME: sscanf
					sprintf(info[i].name, "%s", nm);
				}
			}
		}
		fclose(fp);
	}
	else {
		fprintf(stderr, "failed to open /proc/mtd\n");
		return -1;
	}

	total_sz = 0;
	for (i = 0; i < NUM_INFO+1; i++) {
		total_sz += info[i].size;
	}

#ifdef DEBUG
	printf("dev  size     erasesize name\n"); 
	for (i = 0; i < NUM_INFO; i++) {
		if (info[i].dev[0] != 0)
			printf("%s %08x %08x  %s\n", info[i].dev, info[i].size,
					info[i].erasesize, info[i].name);
	}
	printf("total size: %x\n", total_sz);
#endif
	return 0;
}

int mtd_open(int num, int flags)
{
	char dev[10];
	snprintf(dev, sizeof(dev), "/dev/mtd%d", num);
	return open(dev, flags);
}

int flash_read(int offset, int count)
{
	int i, o, off, cnt, addr, fd, len;
	unsigned char *buf, *p;
	unsigned int result;

#ifdef DEBUG
	printf("%s: offset %x, count %d\n", __func__, offset, count);
#endif
	buf = (unsigned char *)malloc(count);
	if (buf == NULL) {
		fprintf(stderr, "fail to alloc memory for %d bytes\n", count);
		return -1;
	}
	p = buf;
	cnt = count;
	off = offset;

	for (i = 0, addr = 0; i < NUM_INFO; i++) {
		if (addr <= off && off < addr + info[i].size) {
			o = off - addr;
			fd = mtd_open(i, O_RDONLY | O_SYNC);
			if (fd < 0) {
				fprintf(stderr, "failed to open mtd%d\n", i);
				free(buf);
				return -1;
			}
			lseek(fd, o, SEEK_SET);
			len = ((o + cnt) < info[i].size)? cnt : (info[i].size - o);
#ifdef DEBUG
			printf("  read from mtd%d: o %x, len %d\n", i, o, len);
#endif
			read(fd, p, len);
			close(fd);
			cnt -= len;
			if (cnt == 0)
				break;
			off += len;
			p += len;
		}
		addr += info[i].size;
	}
	for (i = 0, p = buf; i < count; i++, p++) {
#if 1 //backward compatibility
		//printf("%X: %X\n", offset + i, *p);
		result = *p;
#else
		printf("%02x", *p);
		if (i % 2 == 1) printf(" ");
#endif
	}
	free(buf);
	return result;
}

static unsigned int getFlash(char *addr)
{
	int offset = 0;
	offset = strtol(addr, NULL, 16);
	return flash_read(offset, 1);
}

char getChar(int value)
{
	if (value == 0)
		return '0';
	else if (value == 1)
		return '1';
	else if (value == 2)
		return '2';
	else if (value == 3)
		return '3';
	else if (value == 4)
		return '4';
	else if (value == 5)
		return '5';
	else if (value == 6)
		return '6';
	else if (value == 7)
		return '7';
	else if (value == 8)
		return '8';
	else if (value == 9)
		return '9';
	else if (value == 10)
		return 'A';
	else if (value == 11)
		return 'B';
	else if (value == 12)
		return 'C';
	else if (value == 13)
		return 'D';
	else if (value == 14)
		return 'E';
	else if (value == 15)
		return 'F';
	
}

int main (int argc, char *argv[])
{
	char BOARDID[6];
	int loop=0;
	FILE *fp;

	if (init_info())
		exit(EXIT_FAILURE);

#ifdef CONFIG_RALINK_RT3883
	BOARDID[0] = getChar((getFlash("401d0") & 0x00F0) >> 4);
	BOARDID[1] = getChar((getFlash("401d0") & 0x000F));
	BOARDID[2] = getChar((getFlash("401d1") & 0x00F0) >> 4);
	BOARDID[3] = getChar((getFlash("401d1") & 0x000F));
	BOARDID[4] = getChar((getFlash("401d2") & 0x00F0) >> 4);
	BOARDID[5] = getChar((getFlash("401d2") & 0x000F));
#else
	BOARDID[0] = getChar((getFlash("400d0") & 0x00F0) >> 4);
	BOARDID[1] = getChar((getFlash("400d0") & 0x000F));
	BOARDID[2] = getChar((getFlash("400d1") & 0x00F0) >> 4);
	BOARDID[3] = getChar((getFlash("400d1") & 0x000F));
	BOARDID[4] = getChar((getFlash("400d2") & 0x00F0) >> 4);
	BOARDID[5] = getChar((getFlash("400d2") & 0x000F));
#endif

	fp = fopen("/var/boardid", "w");
	if (fp!=NULL)
	{
		for (loop=0; loop<6; loop++)
			fputc(BOARDID[loop], fp);
		fclose(fp);
	}
}

