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
 * Mini reset implementation for busybox
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 * Written by Erik Andersen and Kent Robotti <robotti@metconnect.com>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

/* BTW, which "standard" package has this utility? It doesn't seem
 * to be ncurses, coreutils, console-tools... then what? */

#if ENABLE_STTY
int stty_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
#endif

int reset_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int reset_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	static const char *const args[] = {
		"stty", "sane", NULL
	};

	/* no options, no getopt */

	if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO)) {
		/* See 'man 4 console_codes' for details:
		 * "ESC c"			-- Reset
		 * "ESC ( K"		-- Select user mapping
		 * "ESC [ J"		-- Erase display
		 * "ESC [ 0 m"		-- Reset all display attributes
		 * "ESC [ ? 25 h"	-- Make cursor visible.
		 */
		printf("\033c\033(K\033[J\033[0m\033[?25h");
		/* http://bugs.busybox.net/view.php?id=1414:
		 * people want it to reset echo etc: */
#if ENABLE_STTY
		return stty_main(2, (char**)args);
#else
		execvp("stty", (char**)args);
#endif
	}
	return EXIT_SUCCESS;
}
