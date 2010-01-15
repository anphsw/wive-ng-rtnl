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
 * pivot_root.c - Change root file system.  Based on util-linux 2.10s
 *
 * busyboxed by Evin Robertson
 * pivot_root syscall stubbed by Erik Andersen, so it will compile
 *     regardless of the kernel being used.
 *
 * Licensed under GPL version 2, see file LICENSE in this tarball for details.
 */
#include "libbb.h"

extern int pivot_root(const char * new_root,const char * put_old);

int pivot_root_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int pivot_root_main(int argc, char **argv)
{
	if (argc != 3)
		bb_show_usage();

	if (pivot_root(argv[1], argv[2]) < 0) {
		/* prints "pivot_root: <strerror text>" */
		bb_perror_nomsg_and_die();
	}

	return EXIT_SUCCESS;
}
