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
#ifdef	linux
/*
 * flushdisk() - linux block cache clearing
 */

#include	<stdio.h>
#include	<sys/types.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<sys/ioctl.h>
#include	<sys/mount.h>

int
flushdisk(int fd)
{
	int	ret = ioctl(fd, BLKFLSBUF, 0);
	usleep(100000);
	return (ret);
}

#endif

#ifdef	MAIN
int
main(int ac, char **av)
{
#ifdef	linux
	int	fd;
	int	i;

	for (i = 1; i < ac; ++i) {
		fd = open(av[i], 0);
		if (flushdisk(fd)) {
			exit(1);
		}
		close(fd);
	}
#endif
	exit(0);
}
#endif
