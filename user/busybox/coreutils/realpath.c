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

/* BB_AUDIT SUSv3 N/A -- Apparently a busybox extension. */

/* Mar 16, 2003      Manuel Novoa III   (mjn3@codepoet.org)
 *
 * Now does proper error checking on output and returns a failure exit code
 * if one or more paths cannot be resolved.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

int realpath_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int realpath_main(int argc UNUSED_PARAM, char **argv)
{
	int retval = EXIT_SUCCESS;

#if PATH_MAX > (BUFSIZ+1)
	RESERVE_CONFIG_BUFFER(resolved_path, PATH_MAX);
# define resolved_path_MUST_FREE 1
#else
#define resolved_path bb_common_bufsiz1
# define resolved_path_MUST_FREE 0
#endif

	if (!*++argv) {
		bb_show_usage();
	}

	do {
		if (realpath(*argv, resolved_path) != NULL) {
			puts(resolved_path);
		} else {
			retval = EXIT_FAILURE;
			bb_simple_perror_msg(*argv);
		}
	} while (*++argv);

#if ENABLE_FEATURE_CLEAN_UP && resolved_path_MUST_FREE
	RELEASE_CONFIG_BUFFER(resolved_path);
#endif

	fflush_stdout_and_exit(retval);
}
