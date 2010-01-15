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
 * Utility routines.
 *
 * Copyright (C) 2008 by Denys Vlasenko <vda.linux@googlemail.com>
 *
 * Licensed under GPLv2, see file LICENSE in this tarball for details.
 */

/* Keeping it separate allows to NOT suck in stdio for VERY small applets.
 * Try building busybox with only "true" enabled... */

#include "libbb.h"

int die_sleep;
#if ENABLE_FEATURE_PREFER_APPLETS || ENABLE_HUSH
jmp_buf die_jmp;
#endif

void FAST_FUNC xfunc_die(void)
{
	if (die_sleep) {
		if ((ENABLE_FEATURE_PREFER_APPLETS || ENABLE_HUSH)
		 && die_sleep < 0
		) {
			/* Special case. We arrive here if NOFORK applet
			 * calls xfunc, which then decides to die.
			 * We don't die, but jump instead back to caller.
			 * NOFORK applets still cannot carelessly call xfuncs:
			 * p = xmalloc(10);
			 * q = xmalloc(10); // BUG! if this dies, we leak p!
			 */
			/* -2222 means "zero" (longjmp can't pass 0)
			 * run_nofork_applet() catches -2222. */
			longjmp(die_jmp, xfunc_error_retval ? xfunc_error_retval : -2222);
		}
		sleep(die_sleep);
	}
	exit(xfunc_error_retval);
}
