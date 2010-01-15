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
 * Mini pwd implementation for busybox
 *
 * Copyright (C) 1995, 1996 by Bruce Perens <bruce@pixar.com>.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

/* This is a NOFORK applet. Be very careful! */

int pwd_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int pwd_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	char *buf;

	buf = xrealloc_getcwd_or_warn(NULL);
	if (buf != NULL) {
		puts(buf);
		free(buf);
		return fflush(stdout);
	}

	return EXIT_FAILURE;
}
