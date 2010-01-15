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

FILE* FAST_FUNC fopen_or_warn(const char *path, const char *mode)
{
	FILE *fp = fopen(path, mode);
	if (!fp) {
		bb_simple_perror_msg(path);
		errno = 0;
	}
	return fp;
}

FILE* FAST_FUNC fopen_for_read(const char *path)
{
	return fopen(path, "r");
}

FILE* FAST_FUNC xfopen_for_read(const char *path)
{
	return xfopen(path, "r");
}

FILE* FAST_FUNC fopen_for_write(const char *path)
{
	return fopen(path, "w");
}

FILE* FAST_FUNC xfopen_for_write(const char *path)
{
	return xfopen(path, "w");
}
