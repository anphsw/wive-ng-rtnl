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
 * bb_get_last_path_component implementation for busybox
 *
 * Copyright (C) 2001  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
/*
 * "/"        -> "/"
 * "abc"      -> "abc"
 * "abc/def"  -> "def"
 * "abc/def/" -> ""
 */
char* FAST_FUNC bb_get_last_path_component_nostrip(const char *path)
{
	char *slash = strrchr(path, '/');

	if (!slash || (slash == path && !slash[1]))
		return (char*)path;

	return slash + 1;
}

/*
 * "/"        -> "/"
 * "abc"      -> "abc"
 * "abc/def"  -> "def"
 * "abc/def/" -> "def" !!
 */
char* FAST_FUNC bb_get_last_path_component_strip(char *path)
{
	char *slash = last_char_is(path, '/');

	if (slash)
		while (*slash == '/' && slash != path)
			*slash-- = '\0';

	return bb_get_last_path_component_nostrip(path);
}
