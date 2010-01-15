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
 * lat_cmd.c - time to complete a given command line
 *
 * usage: lat_cmd [-P <parallelism>] [-W <warmup>] [-N <repetitions>] cmd...
 *
 * Copyright (c) 2004 Carl Staelin. Distributed under the FSF GPL with
 * additional restriction that results may published only if
 * (1) the benchmark is unmodified, and
 * (2) the version in the sccsid below is included in the report.
 * Support for this development by Sun Microsystems is gratefully acknowledged.
 */
char	*id = "$Id: lat_cmd.c,v 1.1.1.1 2006-11-23 11:25:34 steven Exp $\n";

#include "bench.h"

void bench(iter_t iterations, void *cookie);
void cleanup(iter_t iterations, void *cookie);

typedef struct _state {
	char**	argv;
	pid_t	pid;
} state_t;

int 
main(int ac, char **av)
{
	int c;
	int i;
	int parallel = 1;
	int warmup = 0;
	int repetitions = TRIES;
	char buf[1024];
	state_t state;
	char* usage = "[-P <parallelism>] [-W <warmup>] [-N <repetitions>] cmdline...\n";

	while (( c = getopt(ac, av, "P:W:N:")) != EOF) {
		switch(c) {
		case 'P':
			parallel = atoi(optarg);
			if (parallel <= 0) lmbench_usage(ac, av, usage);
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
	if (optind >= ac) {
		lmbench_usage(ac, av, usage);
	}
	state.argv = (char**)malloc((ac - optind + 1) * sizeof(char*));
	state.pid = 0;
	for (i = 0; i < ac - optind; ++i) {
		state.argv[i] = av[optind + i];
	}
	state.argv[i] = NULL;

	benchmp(NULL, bench, NULL, 0, parallel, warmup, repetitions, &state);
	micro("lat_cmd", get_n());
	return (0);
}

void
cleanup(iter_t iterations, void* cookie)
{
	state_t* state = (state_t*)cookie;

	if (iterations) return;

	if (state->pid) {
		kill(state->pid, SIGKILL);
		waitpid(state->pid, NULL, 0);
		state->pid = 0;
	}
}
	
void 
bench(register iter_t iterations, void *cookie)
{
	state_t *state = (state_t *) cookie;

	signal(SIGCHLD, SIG_DFL);
	while (iterations-- > 0) {
		switch (state->pid = fork()) {
		case '0':
			execvp(state->argv[0], state->argv);
			/*NOTREACHED*/
		default:
			break;
		}
		waitpid(state->pid, NULL, 0);
		state->pid = 0;
	}
}

