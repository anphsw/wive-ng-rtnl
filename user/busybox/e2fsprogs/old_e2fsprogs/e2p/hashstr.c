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
 * feature.c --- convert between features and strings
 *
 * Copyright (C) 1999  Theodore Ts'o <tytso@mit.edu>
 *
 * This file can be redistributed under the terms of the GNU Library General
 * Public License
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "e2p.h"

struct hash {
	int num;
	const char *string;
};

static const struct hash hash_list[] = {
	{ EXT2_HASH_LEGACY,   "legacy" },
	{ EXT2_HASH_HALF_MD4, "half_md4" },
	{ EXT2_HASH_TEA,      "tea" },
	{ 0, 0 },
};

const char *e2p_hash2string(int num)
{
	const struct hash *p;
	static char buf[20];

	for (p = hash_list; p->string; p++) {
		if (num == p->num)
			return p->string;
	}
	sprintf(buf, "HASHALG_%d", num);
	return buf;
}

/*
 * Returns the hash algorithm, or -1 on error
 */
int e2p_string2hash(char *string)
{
	const struct hash *p;
	char *eptr;
	int num;

	for (p = hash_list; p->string; p++) {
		if (!strcasecmp(string, p->string)) {
			return p->num;
		}
	}
	if (strncasecmp(string, "HASHALG_", 8))
		return -1;

	if (string[8] == 0)
		return -1;
	num = strtol(string+8, &eptr, 10);
	if (num > 255 || num < 0)
		return -1;
	if (*eptr)
		return -1;
	return num;
}
