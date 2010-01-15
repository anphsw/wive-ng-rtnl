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
 * crypt_make_salt
 *
 * i64c was also put here, this is the only function that uses it.
 *
 * Lifted from loginutils/passwd.c by Thomas Lundquist <thomasez@zelow.no>
 *
 */

#include "libbb.h"

static int i64c(int i)
{
	i &= 0x3f;
	if (i == 0)
		return '.';
	if (i == 1)
		return '/';
	if (i < 12)
		return ('0' - 2 + i);
	if (i < 38)
		return ('A' - 12 + i);
	return ('a' - 38 + i);
}

int FAST_FUNC crypt_make_salt(char *p, int cnt, int x)
{
	x += getpid() + time(NULL);
	do {
		/* x = (x*1664525 + 1013904223) % 2^32 generator is lame
		 * (low-order bit is not "random", etc...),
		 * but for our purposes it is good enough */
		x = x*1664525 + 1013904223;
		/* BTW, Park and Miller's "minimal standard generator" is
		 * x = x*16807 % ((2^31)-1)
		 * It has no problem with visibly alternating lowest bit
		 * but is also weak in cryptographic sense + needs div,
		 * which needs more code (and slower) on many CPUs */
		*p++ = i64c(x >> 16);
		*p++ = i64c(x >> 22);
	} while (--cnt);
	*p = '\0';
	return x;
}
