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
 * wfopen_input implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* A number of applets need to open a file for reading, where the filename
 * is a command line arg.  Since often that arg is '-' (meaning stdin),
 * we avoid testing everywhere by consolidating things in this routine.
 */

#include "libbb.h"

FILE* FAST_FUNC fopen_or_warn_stdin(const char *filename)
{
	FILE *fp = stdin;

	if (filename != bb_msg_standard_input
	 && NOT_LONE_DASH(filename)
	) {
		fp = fopen_or_warn(filename, "r");
	}
	return fp;
}

FILE* FAST_FUNC xfopen_stdin(const char *filename)
{
	FILE *fp = fopen_or_warn_stdin(filename);
	if (fp)
		return fp;
	xfunc_die();	/* We already output an error message. */
}

int FAST_FUNC open_or_warn_stdin(const char *filename)
{
	int fd = STDIN_FILENO;

	if (filename != bb_msg_standard_input
	 && NOT_LONE_DASH(filename)
	) {
		fd = open_or_warn(filename, O_RDONLY);
	}

	return fd;
}
