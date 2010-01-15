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
 * Definitions for the CHU line discipline v2.0
 */

/*
 * The CHU time code consists of 10 BCD digits and is repeated
 * twice for a total of 10 characters.  A time is taken after
 * the arrival of each character.  The following structure is
 * used to return this stuff.
 */
#define	NCHUCHARS	(10)

struct chucode {
	u_char codechars[NCHUCHARS];	/* code characters */
	u_char ncodechars;		/* number of code characters */
	u_char chutype;			/* packet type */
	struct timeval codetimes[NCHUCHARS];	/* arrival times */
};

#define CHU_TIME 0		/* second half is equal to first half */
#define CHU_YEAR 1		/* second half is one's complement */

