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
 * FILE lock.c
 *
 * This utility locks one or more sectors of flash device.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <string.h>
#include "mtd/mtd-user.h"

int main(int argc, char *argv[])
{
  int fd;
  struct mtd_info_user mtdInfo;
  struct erase_info_user mtdLockInfo;
  int num_sectors;
  int ofs;

  /*
   * Parse command line options
   */
  if(argc != 4)
  {
    fprintf(stderr, "USAGE: %s <mtd device> <ofs in hex> <num of sectors in decimal or -1 for all sectors>\n", argv[0]);
    exit(1);
  }
  else if(strncmp(argv[1], "/dev/mtd", 8) != 0)
  {
    fprintf(stderr, "'%s' is not a MTD device.  Must specify mtd device: /dev/mtd?\n", argv[1]);
    exit(1);
  }

  fd = open(argv[1], O_RDWR);
  if(fd < 0)
  {
    fprintf(stderr, "Could not open mtd device: %s\n", argv[1]);
    exit(1);
  }

  if(ioctl(fd, MEMGETINFO, &mtdInfo))
  {
    fprintf(stderr, "Could not get MTD device info from %s\n", argv[1]);
    close(fd);
    exit(1);
  }
  sscanf(argv[2], "%x",&ofs);
  sscanf(argv[3], "%d",&num_sectors);
  if(ofs > mtdInfo.size - mtdInfo.erasesize)
  {
    fprintf(stderr, "%x is beyond device size %x\n",ofs,(unsigned int)(mtdInfo.size - mtdInfo.erasesize));
    exit(1);
  }

  if (num_sectors == -1) {
	  num_sectors = mtdInfo.size/mtdInfo.erasesize;
  }
  else {
	  if(num_sectors > mtdInfo.size/mtdInfo.erasesize)
	  {
	    fprintf(stderr, "%d are too many sectors, device only has %d\n",num_sectors,(int)(mtdInfo.size/mtdInfo.erasesize));
	    exit(1);
	  }
  }

  mtdLockInfo.start = ofs;
  mtdLockInfo.length = num_sectors * mtdInfo.erasesize;
  if(ioctl(fd, MEMLOCK, &mtdLockInfo))
  {
    fprintf(stderr, "Could not lock MTD device: %s\n", argv[1]);
    close(fd);
    exit(1);
  }

  return 0;
}

