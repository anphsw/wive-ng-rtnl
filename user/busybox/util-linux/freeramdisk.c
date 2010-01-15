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
 * freeramdisk and fdflush implementations for busybox
 *
 * Copyright (C) 2000 and written by Emanuele Caratti <wiz@iol.it>
 * Adjusted a bit by Erik Andersen <andersen@codepoet.org>
 * Unified with fdflush by Tito Ragusa <farmatito@tiscali.it>
 *
 * Licensed under GPLv2, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

/* From <linux/fd.h> */
#define FDFLUSH  _IO(2,0x4b)

int freeramdisk_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int freeramdisk_main(int argc, char **argv)
{
	int fd;

	if (argc != 2) bb_show_usage();

	fd = xopen(argv[1], O_RDWR);

	// Act like freeramdisk, fdflush, or both depending on configuration.
	ioctl_or_perror_and_die(fd, (ENABLE_FREERAMDISK && applet_name[1]=='r')
			|| !ENABLE_FDFLUSH ? BLKFLSBUF : FDFLUSH, NULL, "%s", argv[1]);

	if (ENABLE_FEATURE_CLEAN_UP) close(fd);

	return EXIT_SUCCESS;
}
