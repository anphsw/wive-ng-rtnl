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
/*
 * hextoint - convert an ascii string in hex to an unsigned
 *	      long, with error checking
 */
#include <ctype.h>

#include "ntp_stdlib.h"

int
hextoint(
	const char *str,
	u_long *ival
	)
{
	register u_long u;
	register const char *cp;

	cp = str;

	if (*cp == '\0')
	    return 0;

	u = 0;
	while (*cp != '\0') {
		if (!isxdigit((int)*cp))
		    return 0;
		if (u >= 0x10000000)
		    return 0;	/* overflow */
		u <<= 4;
		if (*cp <= '9')		/* very ascii dependent */
		    u += *cp++ - '0';
		else if (*cp >= 'a')
		    u += *cp++ - 'a' + 10;
		else
		    u += *cp++ - 'A' + 10;
	}
	*ival = u;
	return 1;
}
