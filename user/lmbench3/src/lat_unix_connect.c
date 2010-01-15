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
 * lat_unix_connect.c - simple UNIX connection latency test
 *
 * Three programs in one -
 *	server usage:	lat_connect -s
 *	client usage:	lat_connect [-P <parallelism>] [-W <warmup>] [-N <repetitions>]
 *	shutdown:	lat_connect -q
 *
 * Copyright (c) 1994 Larry McVoy.  Distributed under the FSF GPL with
 * additional restriction that results may published only if
 * (1) the benchmark is unmodified, and
 * (2) the version in the sccsid below is included in the report.
 * Support for this development by Sun Microsystems is gratefully acknowledged.
 */
char	*id = "$Id: lat_unix_connect.c,v 1.1.1.1 2006-11-23 11:25:34 steven Exp $\n";
#include "bench.h"

#define CONNAME "/tmp/af_unix"

void server_main(void);

void benchmark(iter_t iterations, void* cookie)
{
	while (iterations-- > 0) {
		int	sock = unix_connect(CONNAME);
		if (sock <= 0)
			printf("error on iteration %lu\n",iterations);
		close(sock);
	}
}

int main(int ac, char **av)
{
	int parallel = 1;
	int warmup = 0;
	int repetitions = TRIES;
	char	*usage = "-s\n OR [-P <parallelism>] [-W <warmup>] [-N <repetitions>]\n OR -S\n";
	int	c;

	/* Start the server "-s" or Shut down the server "-S" */
	if (ac == 2) {
		if (!strcmp(av[1], "-s")) {
			if (fork() == 0) {
				server_main();
			}
			exit(0);
		}
		if (!strcmp(av[1], "-S")) {
			int sock = unix_connect(CONNAME);
			write(sock, "0", 1);
			close(sock);
			exit(0);
		}
	}

	/*
	 * Rest is client
	 */
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

	if (optind != ac) {
		lmbench_usage(ac, av, usage);
	}

	benchmp(NULL, benchmark, NULL, 0, parallel, warmup, repetitions, NULL);
	micro("UNIX connection cost", get_n());
}

void server_main(void)
{
	int     newsock, sock;
	char	c;

	GO_AWAY;
	sock = unix_server(CONNAME);
	for (;;) {
		newsock = unix_accept(sock);
		c = 0;
		read(newsock, &c, 1);
		if (c && c == '0') {
			unix_done(sock, CONNAME);
			exit(0);
		}
		close(newsock);
	}
}
