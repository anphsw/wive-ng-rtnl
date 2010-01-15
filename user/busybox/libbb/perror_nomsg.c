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
 * bb_perror_nomsg implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* gcc warns about a null format string, therefore we provide
 * modified definition without "attribute (format)"
 * instead of including libbb.h */
//#include "libbb.h"
#include "platform.h"
extern void bb_perror_msg(const char *s, ...) FAST_FUNC;

/* suppress gcc "no previous prototype" warning */
void FAST_FUNC bb_perror_nomsg(void);
void FAST_FUNC bb_perror_nomsg(void)
{
	bb_perror_msg(0);
}
