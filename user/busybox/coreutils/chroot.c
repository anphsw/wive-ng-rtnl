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
 * Mini chroot implementation for busybox
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* BB_AUDIT SUSv3 N/A -- Matches GNU behavior. */

#include "libbb.h"

int chroot_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int chroot_main(int argc, char **argv)
{
	if (argc < 2) {
		bb_show_usage();
	}

	++argv;
	xchroot(*argv);
	xchdir("/");

	++argv;
	if (argc == 2) {
		argv -= 2;
		argv[0] = getenv("SHELL");
		if (!argv[0]) {
			argv[0] = (char *) DEFAULT_SHELL;
		}
		argv[1] = (char *) "-i";
	}

	BB_EXECVP(*argv, argv);
	bb_perror_msg_and_die("cannot execute %s", *argv);
}
