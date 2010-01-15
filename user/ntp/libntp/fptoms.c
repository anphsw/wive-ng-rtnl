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
 * fptoms - return an asciized s_fp number in milliseconds
 */
#include "ntp_fp.h"

char *
fptoms(
	s_fp fpv,
	short ndec
	)
{
	u_fp plusfp;
	int neg;

	if (fpv < 0) {
		plusfp = (u_fp)(-fpv);
		neg = 1;
	} else {
		plusfp = (u_fp)fpv;
		neg = 0;
	}

	return dofptoa(plusfp, neg, ndec, 1);
}
