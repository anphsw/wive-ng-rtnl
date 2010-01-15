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
/* Print string that matches bit masked flags
 *
 * Copyright (C) 2008 Natanael Copa <natanael.copa@gmail.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include <libbb.h>

/* returns a set with the flags not printed */
int FAST_FUNC print_flags_separated(const int *masks, const char *labels, int flags, const char *separator)
{
	const char *need_separator = NULL;
	while (*labels) {
		if (flags & *masks) {
			printf("%s%s",
				need_separator ? need_separator : "",
				labels);
			need_separator = separator;
			flags &= ~ *masks;
		}
		masks++;
		labels += strlen(labels) + 1;
	}
	return flags;
}

int FAST_FUNC print_flags(const masks_labels_t *ml, int flags)
{
	return print_flags_separated(ml->masks, ml->labels, flags, NULL);
}
