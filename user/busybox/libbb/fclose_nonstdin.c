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
 * fclose_nonstdin implementation for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* A number of standard utilities can accept multiple command line args
 * of '-' for stdin, according to SUSv3.  So we encapsulate the check
 * here to save a little space.
 */

#include "libbb.h"

int FAST_FUNC fclose_if_not_stdin(FILE *f)
{
	/* Some more paranoid applets want ferror() check too */
	int r = ferror(f); /* NB: does NOT set errno! */
	if (r) errno = EIO; /* so we'll help it */
	if (f != stdin)
		return (r | fclose(f)); /* fclose does set errno on error */
	return r;
}
