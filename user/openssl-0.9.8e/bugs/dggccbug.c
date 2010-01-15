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
/* NOCW */
/* dggccbug.c */
/* bug found by Eric Young (eay@cryptsoft.com) - May 1995 */

#include <stdio.h>

/* There is a bug in
 * gcc version 2.5.8 (88open OCS/BCS, DG-2.5.8.3, Oct 14 1994)
 * as shipped with DGUX 5.4R3.10 that can be bypassed by defining
 * DG_GCC_BUG in my code.
 * The bug manifests itself by the vaule of a pointer that is
 * used only by reference, not having it's value change when it is used
 * to check for exiting the loop.  Probably caused by there being 2
 * copies of the valiable, one in a register and one being an address
 * that is passed. */

/* compare the out put from
 * gcc dggccbug.c; ./a.out
 * and
 * gcc -O dggccbug.c; ./a.out
 * compile with -DFIXBUG to remove the bug when optimising.
 */

void inc(a)
int *a;
	{
	(*a)++;
	}

main()
	{
	int p=0;
#ifdef FIXBUG
	int dummy;
#endif

	while (p<3)
		{
		fprintf(stderr,"%08X\n",p);
		inc(&p);
#ifdef FIXBUG
		dummy+=p;
#endif
		}
	}
