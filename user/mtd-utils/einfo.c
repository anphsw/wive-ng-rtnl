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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include "mtd/mtd-user.h"

int main(int argc,char *argv[])
{
	int regcount;
	int Fd;

	if (1 >= argc)
	{
		fprintf(stderr,"You must specify a device\n");
		return 16;
	}
   
	// Open and size the device
	if ((Fd = open(argv[1],O_RDWR)) < 0)
	{
		fprintf(stderr,"File open error\n");
		return 8;
	}

	if (ioctl(Fd,MEMGETREGIONCOUNT,&regcount) == 0)
	{
		int i;
		region_info_t reginfo;
		printf("Device %s has %d erase regions\n", argv[1], regcount);
		for (i = 0; i < regcount; i++)
		{
			reginfo.regionindex = i;
			if(ioctl(Fd, MEMGETREGIONINFO, &reginfo) == 0)
			{
				printf("Region %d is at 0x%x with size 0x%x and "
				       "has 0x%x blocks\n", i, reginfo.offset,
				       reginfo.erasesize, reginfo.numblocks);
			}
			else
			{
				printf("Strange can not read region %d from a %d region device\n",
				       i, regcount);
			}
		}
	}
	return 0;
}
