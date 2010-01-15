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
char	*id = "$Id: seek.c,v 1.1.1.1 2006-11-23 11:25:34 steven Exp $\n";
/*
 * Seek - calculate seeks as a function of distance.
 *
 * Usage: seek file size
 *
 * Copyright (c) 1994,1995,1996 Larry McVoy.  All rights reserved.
 */

#include	"bench.h"

#define	STRIDE	1024*1024

main(ac, av)
	int	ac;
	char  	*av[];
{
	char	buf[512];
	int	disk;
	off_t	size;
	off_t	begin, end;
	int	usecs;

	if (ac != 3) {
		exit(1);
	}
	if ((disk = open(av[1], 0)) == -1) {
		exit(1);
	}
	size = atol(av[2]);
	switch (av[2][strlen(av[2])-1]) {
	    case 'k':	size <<= 10;		break;
	    case 'K':	size *= 1000;		break;
	    case 'm':	size <<= 20;		break;
	    case 'M':	size *= 1000000;	break;
	    case 'g':	size <<= 30;		break;
	    case 'G':	size *= 1000000000L;	break;
	}

	/*
	 * We flip back and forth, in strides of 1MB.
	 * If we have a 100MB disk, that means we do
	 * 1, 99, 2, 98, etc.
	 */
	end = size;
	begin = 0;
	lseek(disk, begin, 0);
	read(disk, buf, sizeof(buf));
	while (end > begin) {
		end -= STRIDE;
		start();
		lseek(disk, end, 0);
		read(disk, buf, sizeof(buf));
		usecs = stop();
		printf("%.04f %.04f\n", (end - begin) / 1000000., usecs/1000.);

		begin += STRIDE;
		start();
		lseek(disk, begin, 0);
		read(disk, buf, sizeof(buf));
		usecs = stop();
		printf("%.04f %.04f\n", (end - begin) / 1000000., usecs/1000.);
	}
	exit(0);
}
