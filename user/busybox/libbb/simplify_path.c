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
 * bb_simplify_path implementation for busybox
 *
 * Copyright (C) 2001  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

char* FAST_FUNC bb_simplify_path(const char *path)
{
	char *s, *start, *p;

	if (path[0] == '/')
		start = xstrdup(path);
	else {
		s = xrealloc_getcwd_or_warn(NULL);
		start = concat_path_file(s, path);
		free(s);
	}
	p = s = start;

	do {
		if (*p == '/') {
			if (*s == '/') {	/* skip duplicate (or initial) slash */
				continue;
			}
			if (*s == '.') {
				if (s[1] == '/' || !s[1]) {	/* remove extra '.' */
					continue;
				}
				if ((s[1] == '.') && (s[2] == '/' || !s[2])) {
					++s;
					if (p > start) {
						while (*--p != '/')	/* omit previous dir */
							continue;
					}
					continue;
				}
			}
		}
		*++p = *s;
	} while (*++s);

	if ((p == start) || (*p != '/')) {	/* not a trailing slash */
		++p;					/* so keep last character */
	}
	*p = 0;

	return start;
}
