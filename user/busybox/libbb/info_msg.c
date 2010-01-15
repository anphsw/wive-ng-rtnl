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
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
#include <syslog.h>

void FAST_FUNC bb_info_msg(const char *s, ...)
{
	va_list p;
	/* va_copy is used because it is not portable
	 * to use va_list p twice */
	va_list p2;

	va_start(p, s);
	va_copy(p2, p);
	if (logmode & LOGMODE_STDIO) {
		vprintf(s, p);
		fputs(msg_eol, stdout);
	}
	if (ENABLE_FEATURE_SYSLOG && (logmode & LOGMODE_SYSLOG))
		vsyslog(LOG_INFO, s, p2);
	va_end(p2);
	va_end(p);
}
