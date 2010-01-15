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
 * Mini chgrp implementation for busybox
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* BB_AUDIT SUSv3 defects - none? */
/* BB_AUDIT GNU defects - unsupported long options. */
/* http://www.opengroup.org/onlinepubs/007904975/utilities/chgrp.html */

#include "libbb.h"

/* This is a NOEXEC applet. Be very careful! */


int chgrp_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int chgrp_main(int argc, char **argv)
{
	/* "chgrp [opts] abc file(s)" == "chown [opts] :abc file(s)" */
	char **p = argv;
	while (*++p) {
		if (p[0][0] != '-') {
			p[0] = xasprintf(":%s", p[0]);
			break;
		}
	}
	return chown_main(argc, argv);
}
