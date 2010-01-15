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
/*
 * RT2880 read/write register utility.
 *
 * compile: mipsel-linux-gcc mmap.c
 *
 * usage:
 * mmap 0x00001234			-- read mode
 * mmap w 0x00001234 0x1	-- write mode
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <sys/mman.h> 
#include <fcntl.h> 
#include <errno.h>

#define PAGE_SIZE		0x1000 	/* 4096 */

#define READMODE	0x0
#define WRITEMODE	0x1
#define WRITE_DELAY	100			/* ms */

int main(int argc, char *argv[])
{
	int fd; 
	unsigned int addr;
	unsigned int round;
	void *start;
	volatile unsigned int *v_addr;
	long long int new_value;

	int mode;

	if(argc == 2){
		mode = READMODE;
		addr = strtoll(argv[1], NULL, 16);
	}else if(argc == 4){
		mode = WRITEMODE;
		addr = strtoll(argv[2], NULL, 16);
		new_value = strtoll(argv[3], NULL, 16);
		//printf("new_value is 0x%08x\n", new_value);
	}else{
		printf("%s 0x00001234	\t-- read mode\n", argv[0]);
		printf("%s w 0x00001234 0x1\t\t-- write mode\n", argv[0]);
		return;
	}

	fd = open("/dev/mem", O_RDWR | O_SYNC );
	if ( fd < 0 ) { 
		printf("open file /dev/mem error. %s\n", strerror(errno)); 
		return 0; 
	} 

	// round addr to PAGE_SIZE
	round = addr;								// keep old value
	addr = (addr / PAGE_SIZE) * PAGE_SIZE;
	round = round - addr;

	start = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr);
	if(	(int)start == -1 ){
		printf("mmap() failed at phsical address:%d %s\n", addr, strerror(errno)); 
		close(fd);
		return 0;
	}
	//printf("mmap() starts at 0x%08x successfuly\n", (unsigned int) start);

	v_addr = (void *)start + round;
	addr = addr + round;
	printf("0x%08x: 0x%08x\n", addr, *v_addr);

	if(mode == WRITEMODE){
		*v_addr = new_value;
		usleep(WRITE_DELAY * 1000);
		printf("0x%08x: 0x%08x\n", addr, *v_addr);
	}

	munmap(start, PAGE_SIZE);
	close(fd);
	return 0; 
}
