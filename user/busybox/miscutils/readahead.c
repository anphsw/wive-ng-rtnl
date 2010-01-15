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
 * readahead implementation for busybox
 *
 * Preloads the given files in RAM, to reduce access time.
 * Does this by calling the readahead(2) system call.
 *
 * Copyright (C) 2006  Michael Opdenacker <michael@free-electrons.com>
 *
 * Licensed under GPLv2 or later, see file License in this tarball for details.
 */

#include "libbb.h"

int readahead_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int readahead_main(int argc, char **argv)
{
	int retval = EXIT_SUCCESS;

	if (argc == 1) bb_show_usage();

	while (*++argv) {
		int fd = open_or_warn(*argv, O_RDONLY);
		if (fd >= 0) {
			off_t len;
			int r;

			/* fdlength was reported to be unreliable - use seek */
			len = xlseek(fd, 0, SEEK_END);
			xlseek(fd, 0, SEEK_SET);
			r = readahead(fd, 0, len);
			close(fd);
			if (r >= 0)
				continue;
		}
		retval = EXIT_FAILURE;
	}

	return retval;
}
