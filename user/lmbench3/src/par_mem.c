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
 * par_mem.c - determine the memory hierarchy parallelism
 *
 * usage: par_mem [-L <line size>] [-M len[K|M]] [-W <warmup>] [-N <repetitions>]
 *
 * Copyright (c) 2000 Carl Staelin.
 * Copyright (c) 1994 Larry McVoy.  Distributed under the FSF GPL with
 * additional restriction that results may published only if
 * (1) the benchmark is unmodified, and
 * (2) the version in the sccsid below is included in the report.
 * Support for this development by Sun Microsystems is gratefully acknowledged.
 */
char	*id = "$Id: par_mem.c,v 1.1.1.1 2006-11-23 11:25:34 steven Exp $\n";

#include "bench.h"

void compute_times(struct mem_state* state, double* tlb_time, double* cache_time);


/*
 * Assumptions:
 *
 * 1) Cache lines are a multiple of pointer-size words
 * 2) Cache lines are no larger than 1/8 of a page (typically 512 bytes)
 * 3) Pages are an even multiple of cache lines
 */
int
main(int ac, char **av)
{
	int	i;
	int	c;
	int	warmup = 0;
	int	repetitions = TRIES;
	int	print_cost = 0;
	size_t	len;
	size_t	maxlen = 64 * 1024 * 1024;
	double	par;
	struct mem_state state;
	char   *usage = "[-c] [-L <line size>] [-M len[K|M]] [-W <warmup>] [-N <repetitions>]\n";

	state.line = getpagesize() / 16;
	state.pagesize = getpagesize();

	while (( c = getopt(ac, av, "cL:M:W:N:")) != EOF) {
		switch(c) {
		case 'c':
			print_cost = 1;
			break;
		case 'L':
			state.line = atoi(optarg);
			if (state.line < sizeof(char*))
				state.line = sizeof(char*);
			break;
		case 'M':
			maxlen = bytes(optarg);
			break;
		case 'W':
			warmup = atoi(optarg);
			break;
		case 'N':
			repetitions = atoi(optarg);
			break;
		default:
			lmbench_usage(ac, av, usage);
			break;
		}
	}

	for (i = MAX_MEM_PARALLELISM * state.line; i <= maxlen; i<<=1) { 
		par = par_mem(i, warmup, repetitions, &state);

		if (par > 0.) {
			fprintf(stderr, "%.6f %.2f\n", 
				i / (1000. * 1000.), par);
		}
	}

	exit(0);
}


