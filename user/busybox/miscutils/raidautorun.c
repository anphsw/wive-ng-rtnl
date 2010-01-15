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
 * raidautorun implementation for busybox
 *
 * Copyright (C) 2006 Bernhard Fischer
 *
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 *
 */

#include "libbb.h"

#include <linux/major.h>
#include <linux/raid/md_u.h>

int raidautorun_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int raidautorun_main(int argc, char **argv)
{
	if (argc != 2)
		bb_show_usage();

	xioctl(xopen(argv[1], O_RDONLY), RAID_AUTORUN, NULL);

	return EXIT_SUCCESS;
}
