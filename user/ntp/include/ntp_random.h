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

#include <ntp_types.h>

long ntp_random P((void));
void ntp_srandom P((unsigned long));
void ntp_srandomdev P((void));
char * ntp_initstate P((unsigned long, 	/* seed for R.N.G. */
			char *,		/* pointer to state array */
			long		/* # bytes of state info */
			));
char * ntp_setstate P((char *));	/* pointer to state array */



