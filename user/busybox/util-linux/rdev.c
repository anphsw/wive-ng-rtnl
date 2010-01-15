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
 * rdev - print device node associated with a filesystem
 *
 * Copyright (c) 2008 Nuovation System Designs, LLC
 *   Grant Erickson <gerickson@nuovations.com>
 *
 * Licensed under GPLv2, see file LICENSE in this tarball for details.
 *
 */

#include "libbb.h"

int rdev_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int rdev_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	char const * const root_device = find_block_device("/");

	if (root_device != NULL) {
		printf("%s /\n", root_device);
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}
