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
 * printenv implementation for busybox
 *
 * Copyright (C) 2005 by Erik Andersen <andersen@codepoet.org>
 * Copyright (C) 2005 by Mike Frysinger <vapier@gentoo.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

int printenv_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int printenv_main(int argc UNUSED_PARAM, char **argv)
{
	/* no variables specified, show whole env */
	if (!argv[1]) {
		int e = 0;
		while (environ[e])
			puts(environ[e++]);
	} else {
		/* search for specified variables and print them out if found */
		char *arg, *env;

		while ((arg = *++argv) != NULL) {
			env = getenv(arg);
			if (env)
				puts(env);
		}
	}

	fflush_stdout_and_exit(EXIT_SUCCESS);
}
