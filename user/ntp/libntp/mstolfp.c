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
 * mstolfp - convert an ascii string in milliseconds to an l_fp number
 */
#include <stdio.h>
#include <ctype.h>

#include "ntp_fp.h"
#include "ntp_stdlib.h"

int
mstolfp(
	const char *str,
	l_fp *lfp
	)
{
	register const char *cp;
	register char *bp;
	register const char *cpdec;
	char buf[100];

	/*
	 * We understand numbers of the form:
	 *
	 * [spaces][-][digits][.][digits][spaces|\n|\0]
	 *
	 * This is one enormous hack.  Since I didn't feel like
	 * rewriting the decoding routine for milliseconds, what
	 * is essentially done here is to make a copy of the string
	 * with the decimal moved over three places so the seconds
	 * decoding routine can be used.
	 */
	bp = buf;
	cp = str;
	while (isspace((int)*cp))
	    cp++;
	
	if (*cp == '-') {
		*bp++ = '-';
		cp++;
	}

	if (*cp != '.' && !isdigit((int)*cp))
	    return 0;


	/*
	 * Search forward for the decimal point or the end of the string.
	 */
	cpdec = cp;
	while (isdigit((int)*cpdec))
	    cpdec++;

	/*
	 * Found something.  If we have more than three digits copy the
	 * excess over, else insert a leading 0.
	 */
	if ((cpdec - cp) > 3) {
		do {
			*bp++ = (char)*cp++;
		} while ((cpdec - cp) > 3);
	} else {
		*bp++ = '0';
	}

	/*
	 * Stick the decimal in.  If we've got less than three digits in
	 * front of the millisecond decimal we insert the appropriate number
	 * of zeros.
	 */
	*bp++ = '.';
	if ((cpdec - cp) < 3) {
		register int i = 3 - (cpdec - cp);

		do {
			*bp++ = '0';
		} while (--i > 0);
	}

	/*
	 * Copy the remainder up to the millisecond decimal.  If cpdec
	 * is pointing at a decimal point, copy in the trailing number too.
	 */
	while (cp < cpdec)
	    *bp++ = (char)*cp++;
	
	if (*cp == '.') {
		cp++;
		while (isdigit((int)*cp))
		    *bp++ = (char)*cp++;
	}
	*bp = '\0';

	/*
	 * Check to make sure the string is properly terminated.  If
	 * so, give the buffer to the decoding routine.
	 */
	if (*cp != '\0' && !isspace((int)*cp))
	    return 0;
	return atolfp(buf, lfp);
}
