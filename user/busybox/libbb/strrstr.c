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
 * Copyright (C) 2008 Bernhard Fischer
 *
 * Licensed under GPLv2 or later, see file License in this tarball for details.
 */

#ifdef __DO_STRRSTR_TEST
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#else
#include "libbb.h"
#endif

/*
 * The strrstr() function finds the last occurrence of the substring needle
 * in the string haystack. The terminating nul characters are not compared.
 */
char* FAST_FUNC strrstr(const char *haystack, const char *needle)
{
	char *r = NULL;

	if (!needle[0])
		return (char*)haystack + strlen(haystack);
	while (1) {
		char *p = strstr(haystack, needle);
		if (!p)
			return r;
		r = p;
		haystack = p + 1;
	}
}

#ifdef __DO_STRRSTR_TEST
int main(int argc, char **argv)
{
	static const struct {
		const char *h, *n;
		int pos;
	} test_array[] = {
		/* 0123456789 */
		{ "baaabaaab",  "aaa", 5  },
		{ "baaabaaaab", "aaa", 6  },
		{ "baaabaab",   "aaa", 1  },
		{ "aaa",        "aaa", 0  },
		{ "aaa",        "a",   2  },
		{ "aaa",        "bbb", -1 },
		{ "a",          "aaa", -1 },
		{ "aaa",        "",    3  },
		{ "",           "aaa", -1 },
		{ "",           "",    0  },
	};

	int i;

	i = 0;
	while (i < sizeof(test_array) / sizeof(test_array[0])) {
		const char *r = strrstr(test_array[i].h, test_array[i].n);
		printf("'%s' vs. '%s': '%s' - ", test_array[i].h, test_array[i].n, r);
		if (r == NULL)
			r = test_array[i].h - 1;
		printf("%s\n", r == test_array[i].h + test_array[i].pos ? "PASSED" : "FAILED");
		i++;
	}

	return 0;
}
#endif
