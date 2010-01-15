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
 * seq implementation for busybox
 *
 * Copyright (C) 2004, Glenn McGrath
 *
 * Licensed under the GPL v2, see the file LICENSE in this tarball.
 */

#include "libbb.h"

/* This is a NOFORK applet. Be very careful! */


int seq_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int seq_main(int argc, char **argv)
{
	double last, increment, i;

	i = increment = 1;
	switch (argc) {
		case 4:
			increment = atof(argv[2]);
		case 3:
			i = atof(argv[1]);
		case 2:
			last = atof(argv[argc-1]);
			break;
		default:
			bb_show_usage();
	}

	/* You should note that this is pos-5.0.91 semantics, -- FK. */
	while ((increment > 0 && i <= last) || (increment < 0 && i >= last)) {
		printf("%g\n", i);
		i += increment;
	}

	return fflush(stdout);
}
