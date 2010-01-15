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
 * Copyright (C) 2002 by Glenn McGrath
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include <fnmatch.h>
#include "libbb.h"
#include "unarchive.h"

/* Find a string in a shell pattern list */
const llist_t* FAST_FUNC find_list_entry(const llist_t *list, const char *filename)
{
	while (list) {
		if (fnmatch(list->data, filename, 0) == 0) {
			return list;
		}
		list = list->link;
	}
	return NULL;
}

/* Same, but compares only path components present in pattern
 * (extra trailing path components in filename are assumed to match)
 */
const llist_t* FAST_FUNC find_list_entry2(const llist_t *list, const char *filename)
{
	char buf[PATH_MAX];
	int pattern_slash_cnt;
	const char *c;
	char *d;

	while (list) {
		c = list->data;
		pattern_slash_cnt = 0;
		while (*c)
			if (*c++ == '/') pattern_slash_cnt++;
		c = filename;
		d = buf;
		/* paranoia is better than buffer overflows */
		while (*c && d != buf + sizeof(buf)-1) {
			if (*c == '/' && --pattern_slash_cnt < 0)
				break;
			*d++ = *c++;
		}
		*d = '\0';
		if (fnmatch(list->data, buf, 0) == 0) {
			return list;
		}
		list = list->link;
	}
	return NULL;
}
