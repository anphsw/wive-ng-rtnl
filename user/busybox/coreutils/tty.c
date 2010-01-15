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
 * tty implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* BB_AUDIT SUSv3 compliant */
/* http://www.opengroup.org/onlinepubs/007904975/utilities/tty.html */

#include "libbb.h"

int tty_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int tty_main(int argc, char **argv SKIP_INCLUDE_SUSv2(UNUSED_PARAM))
{
	const char *s;
	USE_INCLUDE_SUSv2(int silent;)	/* Note: No longer relevant in SUSv3. */
	int retval;

	xfunc_error_retval = 2;	/* SUSv3 requires > 1 for error. */

	USE_INCLUDE_SUSv2(silent = getopt32(argv, "s");)
	USE_INCLUDE_SUSv2(argc -= optind;)
	SKIP_INCLUDE_SUSv2(argc -= 1;)

	/* gnu tty outputs a warning that it is ignoring all args. */
	bb_warn_ignoring_args(argc);

	retval = 0;

	s = ttyname(0);
	if (s == NULL) {
	/* According to SUSv3, ttyname can fail with EBADF or ENOTTY.
	 * We know the file descriptor is good, so failure means not a tty. */
		s = "not a tty";
		retval = 1;
	}
	USE_INCLUDE_SUSv2(if (!silent) puts(s);)
	SKIP_INCLUDE_SUSv2(puts(s);)

	fflush_stdout_and_exit(retval);
}
