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
 * cat -v implementation for busybox
 *
 * Copyright (C) 2006 Rob Landley <rob@landley.net>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* See "Cat -v considered harmful" at
 * http://cm.bell-labs.com/cm/cs/doc/84/kp.ps.gz */

#include "libbb.h"

int catv_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int catv_main(int argc UNUSED_PARAM, char **argv)
{
	int retval = EXIT_SUCCESS;
	int fd;
	unsigned flags;

	flags = getopt32(argv, "etv");
#define CATV_OPT_e (1<<0)
#define CATV_OPT_t (1<<1)
#define CATV_OPT_v (1<<2)
	flags ^= CATV_OPT_v;
	argv += optind;

	/* Read from stdin if there's nothing else to do. */
	if (!argv[0])
		*--argv = (char*)"-";
	do {
		fd = open_or_warn_stdin(*argv);
		if (fd < 0) {
			retval = EXIT_FAILURE;
			continue;
		}
		for (;;) {
			int i, res;

#define read_buf bb_common_bufsiz1
			res = read(fd, read_buf, COMMON_BUFSIZE);
			if (res < 0)
				retval = EXIT_FAILURE;
			if (res < 1)
				break;
			for (i = 0; i < res; i++) {
				unsigned char c = read_buf[i];

				if (c > 126 && (flags & CATV_OPT_v)) {
					if (c == 127) {
						printf("^?");
						continue;
					}
					printf("M-");
					c -= 128;
				}
				if (c < 32) {
					if (c == 10) {
						if (flags & CATV_OPT_e)
							bb_putchar('$');
					} else if (flags & (c==9 ? CATV_OPT_t : CATV_OPT_v)) {
						printf("^%c", c+'@');
						continue;
					}
				}
				bb_putchar(c);
			}
		}
		if (ENABLE_FEATURE_CLEAN_UP && fd)
			close(fd);
	} while (*++argv);

	fflush_stdout_and_exit(retval);
}
