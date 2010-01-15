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

#include "i2c_drv.h"

char usage[] = 	"i2ccmd read/write eeprom_address data(if write)\n"\
	"i2ccmd format:\n"\
	"  i2ccmd read [address in hex]\n"\
	"  i2ccmd write [size] [address] [value]\n"\
	"NOTE -- size is 1, 2, 4 bytes only, address and value are in hex\n";


int main(int argc, char *argv[])
{
	int chk_match, size, fd;
	unsigned long addr, value;
	int address;
	I2C_WRITE i2c_write;

/* We use the last specified parameters, unless new ones are entered */
	switch (argc) {
		case RT2880_I2C_READ:
			value = 0;
        		chk_match = strcmp(argv[1], RT2880_I2C_READ_STR);
        		if ( chk_match != 0) {
				printf("Usage:\n%s\n", usage);
                		return 1;
        		}
        		addr = strtoul(argv[2], NULL, 16);
			address = addr;

			fd = open("/dev/i2cM0", O_RDONLY); 
			if (fd <= 0) {
				printf("Please insmod module i2c_drv.o!\n");
				return -1;
			}
			ioctl(fd, RT2880_I2C_READ, address);
			close(fd);
        		break;
		case RT2880_I2C_WRITE:
			chk_match = strcmp(argv[1], RT2880_I2C_WRITE_STR);
			if ( chk_match != 0) {
				printf("Usage:\n%s\n", usage);
				return 1;
			}
			size = strtoul(argv[2], NULL, 16);
			addr = strtoul(argv[3], NULL, 16);
			value = strtoul(argv[4], NULL, 16);
			fd = open("/dev/i2cM0", O_RDONLY); 
			i2c_write.address = addr;
			i2c_write.size = size;
			i2c_write.value = value;
			if (fd <= 0) {
				printf("Please insmod module i2c_drv.o!\n");
				return -1;
			}
			ioctl(fd, RT2880_I2C_WRITE, &i2c_write);
			close(fd);
			printf("0x%04x : 0x%08x in %d bytes\n", addr, value, size);
			break;
		default:
			printf("Usage:\n%s\n\n", usage);
	}
	return 0;
}
