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
 * clock.c
 *
 * calculate the minimum timing loop length that gives us significant results
 */
#include "bench.h"

char	*id = "$Id: clock.c,v 1.1.1.1 2006-11-23 11:25:34 steven Exp $";
char	*revision = "$Revision: 1.1.1.1 $";

main()
{
	uint64	enough;
	double	t_overhead, l_overhead;

	enough = compute_enough(15);
	printf("ENOUGH=%lu\n", (unsigned long)enough); fflush(stdout);
	t_overhead = timing_overhead(enough);
	printf("TIMING_OVERHEAD=%f\n", t_overhead); fflush(stdout);
	l_overhead = loop_overhead(enough, t_overhead);
	printf("LOOP_OVERHEAD=%f\n", l_overhead);
	printf("# version [%s]\n", revision);
	exit(0);
}
