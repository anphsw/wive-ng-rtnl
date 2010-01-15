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
 * Mini logname implementation for busybox
 *
 * Copyright (C) 2000  Edward Betts <edward@debian.org>.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* BB_AUDIT SUSv3 compliant */
/* http://www.opengroup.org/onlinepubs/007904975/utilities/logname.html */

/* Mar 16, 2003      Manuel Novoa III   (mjn3@codepoet.org)
 *
 * SUSv3 specifies the string used is that returned from getlogin().
 * The previous implementation used getpwuid() for geteuid(), which
 * is _not_ the same.  Erik apparently made this change almost 3 years
 * ago to avoid failing when no utmp was available.  However, the
 * correct course of action wrt SUSv3 for a failing getlogin() is
 * a diagnostic message and an error return.
 */

#include "libbb.h"

/* This is a NOFORK applet. Be very careful! */

int logname_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int logname_main(int argc, char **argv UNUSED_PARAM)
{
	char buf[128];

	if (argc > 1) {
		bb_show_usage();
	}

	/* Using _r function - avoid pulling in static buffer from libc */
	if (getlogin_r(buf, sizeof(buf)) == 0) {
		puts(buf);
		return fflush(stdout);
	}

	bb_perror_msg_and_die("getlogin");
}
