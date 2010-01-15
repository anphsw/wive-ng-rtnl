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
 * Support functions for mounting devices by label/uuid
 *
 * Copyright (C) 2006 by Jason Schoon <floydpink@gmail.com>
 * Some portions cribbed from e2fsprogs, util-linux, dosfstools
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
#include "volume_id.h"

int findfs_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int findfs_main(int argc, char **argv)
{
	char *tmp = NULL;

	if (argc != 2)
		bb_show_usage();

	if (!strncmp(argv[1], "LABEL=", 6))
		tmp = get_devname_from_label(argv[1] + 6);
	else if (!strncmp(argv[1], "UUID=", 5))
		tmp = get_devname_from_uuid(argv[1] + 5);
	else if (!strncmp(argv[1], "/dev/", 5)) {
		/* Just pass a device name right through.  This might aid in some scripts
		being able to call this unconditionally */
		tmp = argv[1];
	} else
		bb_show_usage();

	if (tmp) {
		puts(tmp);
		return 0;
	}
	return 1;
}
