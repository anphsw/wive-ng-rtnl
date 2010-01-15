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
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

/* returns the array index of the string */
/* (index of first match is returned, or -1) */
int FAST_FUNC index_in_str_array(const char *const string_array[], const char *key)
{
	int i;

	for (i = 0; string_array[i] != 0; i++) {
		if (strcmp(string_array[i], key) == 0) {
			return i;
		}
	}
	return -1;
}

int FAST_FUNC index_in_strings(const char *strings, const char *key)
{
	int idx = 0;

	while (*strings) {
		if (strcmp(strings, key) == 0) {
			return idx;
		}
		strings += strlen(strings) + 1; /* skip NUL */
		idx++;
	}
	return -1;
}

/* returns the array index of the string, even if it matches only a beginning */
/* (index of first match is returned, or -1) */
#ifdef UNUSED
int FAST_FUNC index_in_substr_array(const char *const string_array[], const char *key)
{
	int i;
	int len = strlen(key);
	if (len) {
		for (i = 0; string_array[i] != 0; i++) {
			if (strncmp(string_array[i], key, len) == 0) {
				return i;
			}
		}
	}
	return -1;
}
#endif

int FAST_FUNC index_in_substrings(const char *strings, const char *key)
{
	int len = strlen(key);

	if (len) {
		int idx = 0;
		while (*strings) {
			if (strncmp(strings, key, len) == 0) {
				return idx;
			}
			strings += strlen(strings) + 1; /* skip NUL */
			idx++;
		}
	}
	return -1;
}

const char* FAST_FUNC nth_string(const char *strings, int n)
{
	while (n) {
		n--;
		strings += strlen(strings) + 1;
	}
	return strings;
}
