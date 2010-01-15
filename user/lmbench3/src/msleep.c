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
#include "bench.h"

int
main(int ac, char **av)
{
#if	defined(sgi) || defined(sun) || defined(linux)
	usleep(atoi(av[1]) * 1000);
	return (0);
#else
	fd_set	set;
	int	fd;
	struct	timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = atoi(av[1]) * 1000;
	FD_ZERO(&set);
	FD_SET(0, &set);
	select(1, &set, 0, 0, &tv);
	return (0);
#endif
}
