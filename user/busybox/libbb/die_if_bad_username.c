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
 * Ckeck user and group names for illegal characters
 *
 * Copyright (C) 2008 Tito Ragusa <farmatito@tiscali.it>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

/* To avoid problems, the username should consist only of
 * letters, digits, underscores, periods, at signs and dashes,
 * and not start with a dash (as defined by IEEE Std 1003.1-2001).
 * For compatibility with Samba machine accounts $ is also supported
 * at the end of the username.
 */

void FAST_FUNC die_if_bad_username(const char *name)
{
	goto skip; /* 1st char being dash isn't valid */
	do {
		if (*name == '-')
			continue;
 skip:
		if (isalnum(*name)
		 || *name == '_'
		 || *name == '.'
		 || *name == '@'
		 || (*name == '$' && !*(name + 1))
		) {
			continue;
		}
		bb_error_msg_and_die("illegal character '%c'", *name);
	} while (*++name);
}
