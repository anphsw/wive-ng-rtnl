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
 * mkfifo implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* BB_AUDIT SUSv3 compliant */
/* http://www.opengroup.org/onlinepubs/007904975/utilities/mkfifo.html */

#include "libbb.h"
#include "libcoreutils/coreutils.h"

int mkfifo_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int mkfifo_main(int argc UNUSED_PARAM, char **argv)
{
	mode_t mode;
	int retval = EXIT_SUCCESS;

	mode = getopt_mk_fifo_nod(argv);

	argv += optind;
	if (!*argv) {
		bb_show_usage();
	}

	do {
		if (mkfifo(*argv, mode) < 0) {
			bb_simple_perror_msg(*argv);	/* Avoid multibyte problems. */
			retval = EXIT_FAILURE;
		}
	} while (*++argv);

	return retval;
}
