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
 * Linux32/linux64 allows for changing uname emulation.
 *
 * Copyright 2002 Andi Kleen, SuSE Labs.
 *
 * Licensed under GPL v2 or later, see file License for details.
*/

#include <sys/personality.h>

#include "libbb.h"

int setarch_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int setarch_main(int argc UNUSED_PARAM, char **argv)
{
	int pers = -1;

	/* Figure out what personality we are supposed to switch to ...
	 * we can be invoked as either:
	 * argv[0],argv[1] -> "setarch","personality"
	 * argv[0]         -> "personality"
	 */
retry:
	if (argv[0][5] == '6') /* linux64 */
		pers = PER_LINUX;
	else if (argv[0][5] == '3') /* linux32 */
		pers = PER_LINUX32;
	else if (pers == -1 && argv[1] != NULL) {
		pers = PER_LINUX32;
		++argv;
		goto retry;
	}

	/* make user actually gave us something to do */
	++argv;
	if (argv[0] == NULL)
		bb_show_usage();

	/* Try to set personality */
	if (personality(pers) >= 0) {

		/* Try to execute the program */
		BB_EXECVP(argv[0], argv);
	}

	bb_simple_perror_msg_and_die(argv[0]);
}
