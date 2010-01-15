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
 * usleep implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* BB_AUDIT SUSv3 N/A -- Apparently a busybox extension. */

#include "libbb.h"

/* This is a NOFORK applet. Be very careful! */

int usleep_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int usleep_main(int argc UNUSED_PARAM, char **argv)
{
	if (!argv[1]) {
		bb_show_usage();
	}

	if (usleep(xatou(argv[1]))) {
		bb_perror_nomsg_and_die();
	}

	return EXIT_SUCCESS;
}
