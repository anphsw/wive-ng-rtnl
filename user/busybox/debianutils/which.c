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
 * Which implementation for busybox
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 * Copyright (C) 2006 Gabriel Somlo <somlo at cmu.edu>
 *
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 *
 * Based on which from debianutils
 */

#include "libbb.h"

int which_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int which_main(int argc UNUSED_PARAM, char **argv)
{
	USE_DESKTOP(int opt;)
	int status = EXIT_SUCCESS;
	char *path;
	char *p;

	opt_complementary = "-1"; /* at least one argument */
	USE_DESKTOP(opt =) getopt32(argv, "a");
	argv += optind;

	/* This matches what is seen on e.g. ubuntu.
	 * "which" there is a shell script. */
	path = getenv("PATH");
	if (!path) {
		path = (char*)bb_PATH_root_path;
		putenv(path);
		path += 5; /* skip "PATH=" */
	}

	do {
#if ENABLE_DESKTOP
/* Much bloat just to support -a */
		if (strchr(*argv, '/')) {
			if (execable_file(*argv)) {
				puts(*argv);
				continue;
			}
			status = EXIT_FAILURE;
		} else {
			char *path2 = xstrdup(path);
			char *tmp = path2;

			p = find_execable(*argv, &tmp);
			if (!p)
				status = EXIT_FAILURE;
			else {
 print:
				puts(p);
				free(p);
				if (opt) {
					/* -a: show matches in all PATH components */
					if (tmp) {
						p = find_execable(*argv, &tmp);
						if (p)
							goto print;
					}
				}
			}
			free(path2);
		}
#else
/* Just ignoring -a */
		if (strchr(*argv, '/')) {
			if (execable_file(*argv)) {
				puts(*argv);
				continue;
			}
		} else {
			char *path2 = xstrdup(path);
			char *tmp = path2;
			p = find_execable(*argv, &tmp);
			free(path2);
			if (p) {
				puts(p);
				free(p);
				continue;
			}
		}
		status = EXIT_FAILURE;
#endif
	} while (*(++argv) != NULL);

	fflush_stdout_and_exit(status);
}
