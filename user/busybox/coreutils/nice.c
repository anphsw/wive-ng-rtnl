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
/* vi: set sw=4 ts=4: */
/*
 * nice implementation for busybox
 *
 * Copyright (C) 2005  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include <sys/resource.h>
#include "libbb.h"

int nice_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int nice_main(int argc, char **argv)
{
	int old_priority, adjustment;

	old_priority = getpriority(PRIO_PROCESS, 0);

	if (!*++argv) {	/* No args, so (GNU) output current nice value. */
		printf("%d\n", old_priority);
		fflush_stdout_and_exit(EXIT_SUCCESS);
	}

	adjustment = 10;			/* Set default adjustment. */

	if (argv[0][0] == '-') {
		if (argv[0][1] == 'n') { /* -n */
			if (argv[0][2]) { /* -nNNNN (w/o space) */
				argv[0] += 2; argv--; argc++;
			}
		} else { /* -NNN (NNN may be negative) == -n NNN */
			argv[0] += 1; argv--; argc++;
		}
		if (argc < 4) {			/* Missing priority and/or utility! */
			bb_show_usage();
		}
		adjustment = xatoi_range(argv[1], INT_MIN/2, INT_MAX/2);
		argv += 2;
	}

	{  /* Set our priority. */
		int prio = old_priority + adjustment;

		if (setpriority(PRIO_PROCESS, 0, prio) < 0) {
			bb_perror_msg_and_die("setpriority(%d)", prio);
		}
	}

	BB_EXECVP(*argv, argv);		/* Now exec the desired program. */

	/* The exec failed... */
	xfunc_error_retval = (errno == ENOENT) ? 127 : 126; /* SUSv3 */
	bb_simple_perror_msg_and_die(*argv);
}
