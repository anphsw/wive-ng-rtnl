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
#include <stdio.h>

/* This is a cc optimiser bug for ultrix 4.3, mips CPU.
 * What happens is that the compiler, due to the (a)&7,
 * does
 * i=a&7;
 * i--;
 * i*=4;
 * Then uses i as the offset into a jump table.
 * The problem is that a value of 0 generates an offset of
 * 0xfffffffc.
 */

main()
	{
	f(5);
	f(0);
	}

int f(a)
int a;
	{
	switch(a&7)
		{
	case 7:
		printf("7\n");
	case 6:
		printf("6\n");
	case 5:
		printf("5\n");
	case 4:
		printf("4\n");
	case 3:
		printf("3\n");
	case 2:
		printf("2\n");
	case 1:
		printf("1\n");
#ifdef FIX_BUG
	case 0:
		;
#endif
		}
	}	

