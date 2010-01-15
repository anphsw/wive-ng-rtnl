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
 * mesg implementation for busybox
 *
 * Copyright (c) 2002 Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under the GPL v2 or later, see the file LICENSE in this tarball.
 */

#include "libbb.h"

#ifdef USE_TTY_GROUP
#define S_IWGRP_OR_S_IWOTH	S_IWGRP
#else
#define S_IWGRP_OR_S_IWOTH	(S_IWGRP | S_IWOTH)
#endif

int mesg_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int mesg_main(int argc, char **argv)
{
	struct stat sb;
	const char *tty;
	char c = 0;

	if (--argc == 0
	 || (argc == 1 && ((c = **++argv) == 'y' || c == 'n'))
	) {
		tty = ttyname(STDERR_FILENO);
		if (tty == NULL) {
			tty = "ttyname";
		} else if (stat(tty, &sb) == 0) {
			mode_t m;
			if (argc == 0) {
				puts((sb.st_mode & (S_IWGRP|S_IWOTH)) ? "is y" : "is n");
				return EXIT_SUCCESS;
			}
			m = (c == 'y') ? sb.st_mode | S_IWGRP_OR_S_IWOTH
			               : sb.st_mode & ~(S_IWGRP|S_IWOTH);
			if (chmod(tty, m) == 0) {
				return EXIT_SUCCESS;
			}
		}
		bb_simple_perror_msg_and_die(tty);
	}
	bb_show_usage();
}
