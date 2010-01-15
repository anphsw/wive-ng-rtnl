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
 * Copyright (C) many different people.
 * If you wrote this, please acknowledge your work.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

/* Concatenate path and filename to new allocated buffer.
 * Add '/' only as needed (no duplicate // are produced).
 * If path is NULL, it is assumed to be "/".
 * filename should not be NULL.
 */

#include "libbb.h"

char* FAST_FUNC concat_path_file(const char *path, const char *filename)
{
	char *lc;

	if (!path)
		path = "";
	lc = last_char_is(path, '/');
	while (*filename == '/')
		filename++;
	return xasprintf("%s%s%s", path, (lc==NULL ? "/" : ""), filename);
}
