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
 * setkeycodes
 *
 * Copyright (C) 1994-1998 Andries E. Brouwer <aeb@cwi.nl>
 *
 * Adjusted for BusyBox by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

//#include <sys/ioctl.h>
#include "libbb.h"

/* From <linux/kd.h> */
struct kbkeycode {
	unsigned scancode, keycode;
};
enum {
	KDSETKEYCODE = 0x4B4D  /* write kernel keycode table entry */
};

int setkeycodes_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int setkeycodes_main(int argc, char **argv)
{
	int fd, sc;
	struct kbkeycode a;

	if (!(argc & 1) /* if even */ || argc < 2) {
		bb_show_usage();
	}

	fd = get_console_fd_or_die();

	while (argc > 2) {
		a.keycode = xatou_range(argv[2], 0, 127);
		a.scancode = sc = xstrtoul_range(argv[1], 16, 0, 255);
		if (a.scancode > 127) {
			a.scancode -= 0xe000;
			a.scancode += 128;
		}
		ioctl_or_perror_and_die(fd, KDSETKEYCODE, &a,
			"can't set SCANCODE %x to KEYCODE %d",
			sc, a.keycode);
		argc -= 2;
		argv += 2;
	}
	return EXIT_SUCCESS;
}
