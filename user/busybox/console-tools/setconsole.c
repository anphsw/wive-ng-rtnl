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
 *  setconsole.c - redirect system console output
 *
 *  Copyright (C) 2004,2005  Enrik Berkhan <Enrik.Berkhan@inka.de>
 *  Copyright (C) 2008 Bernhard Fischer
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

int setconsole_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int setconsole_main(int argc UNUSED_PARAM, char **argv)
{
	const char *device = CURRENT_TTY;
	bool reset;

#if ENABLE_FEATURE_SETCONSOLE_LONG_OPTIONS
	static const char setconsole_longopts[] ALIGN1 =
		"reset\0" No_argument "r"
		;
	applet_long_options = setconsole_longopts;
#endif
	/* at most one non-option argument */
	opt_complementary = "?1";
	reset = getopt32(argv, "r");

	argv += 1 + reset;
	if (*argv) {
		device = *argv;
	} else {
		if (reset)
			device = DEV_CONSOLE;
	}

	xioctl(xopen(device, O_RDONLY), TIOCCONS, NULL);
	return EXIT_SUCCESS;
}
