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
 * Disallocate virtual terminal(s)
 *
 * Copyright (C) 2003 by Tito Ragusa <farmatito@tiscali.it>
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* no options, no getopt */

#include "libbb.h"

/* From <linux/vt.h> */
enum { VT_DISALLOCATE = 0x5608 }; /* free memory associated to vt */

int deallocvt_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int deallocvt_main(int argc UNUSED_PARAM, char **argv)
{
	/* num = 0 deallocate all unused consoles */
	int num = 0;

	if (argv[1]) {
		if (argv[2])
			bb_show_usage();
		num = xatou_range(argv[1], 1, 63);
	}

	/* double cast suppresses "cast to ptr from int of different size" */
	xioctl(get_console_fd_or_die(), VT_DISALLOCATE, (void *)(ptrdiff_t)num);
	return EXIT_SUCCESS;
}
