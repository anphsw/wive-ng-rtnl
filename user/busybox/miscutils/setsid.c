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
 * setsid.c -- execute a command in a new session
 * Rick Sladkey <jrs@world.std.com>
 * In the public domain.
 *
 * 1999-02-22 Arkadiusz Mickiewicz <misiek@pld.ORG.PL>
 * - added Native Language Support
 *
 * 2001-01-18 John Fremlin <vii@penguinpowered.com>
 * - fork in case we are process group leader
 *
 * 2004-11-12 Paul Fox
 * - busyboxed
 */

#include "libbb.h"

int setsid_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int setsid_main(int argc UNUSED_PARAM, char **argv)
{
	if (!argv[1])
		bb_show_usage();

	/* setsid() is allowed only when we are not a process group leader.
	 * Otherwise our PID serves as PGID of some existing process group
	 * and cannot be used as PGID of a new process group. */
	if (getpgrp() == getpid())
		forkexit_or_rexec(argv);

	setsid();  /* no error possible */

	BB_EXECVP(argv[1], argv + 1);
	bb_simple_perror_msg_and_die(argv[1]);
}
