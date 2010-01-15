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

/* BB_AUDIT SUSv3 N/A -- Apparently a busybox (obsolete?) extension. */

#include "libbb.h"

/* This is a NOFORK applet. Be very careful! */

int length_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int length_main(int argc, char **argv)
{
	if ((argc != 2) || (**(++argv) == '-')) {
		bb_show_usage();
	}

	printf("%u\n", (unsigned)strlen(*argv));

	return fflush(stdout);
}
