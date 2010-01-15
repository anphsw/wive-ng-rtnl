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
 * fflush_stdout_and_exit implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* Attempt to fflush(stdout), and exit with an error code if stdout is
 * in an error state.
 */

#include "libbb.h"

void FAST_FUNC fflush_stdout_and_exit(int retval)
{
	if (fflush(stdout))
		bb_perror_msg_and_die(bb_msg_standard_output);

	if (ENABLE_FEATURE_PREFER_APPLETS && die_sleep < 0) {
		/* We are in NOFORK applet. Do not exit() directly,
		 * but use xfunc_die() */
		xfunc_error_retval = retval;
		xfunc_die();
	}

	exit(retval);
}
