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
 * Mini losetup implementation for busybox
 *
 * Copyright (C) 2002  Matt Kraai.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

int losetup_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int losetup_main(int argc, char **argv)
{
	char dev[] = LOOP_NAME"0";
	unsigned opt;
	char *opt_o;
	char *s;
	unsigned long long offset = 0;

	/* max 2 args, all opts are mutually exclusive */
	opt_complementary = "?2:d--of:o--df:f-do";
	opt = getopt32(argv, "do:f", &opt_o);
	argc -= optind;
	argv += optind;

	if (opt == 0x2) // -o
		offset = xatoull(opt_o);

	if (opt == 0x4 && argc) // -f does not take any argument
		bb_show_usage();

	if (opt == 0x1) { // -d
		/* detach takes exactly one argument */
		if (argc != 1)
			bb_show_usage();
		if (del_loop(argv[0]))
			bb_simple_perror_msg_and_die(argv[0]);
		return EXIT_SUCCESS;
	}

	if (argc == 2) {
		/* -o or no option */
		if (set_loop(&argv[0], argv[1], offset) < 0)
			bb_simple_perror_msg_and_die(argv[0]);
		return EXIT_SUCCESS;
	}

	if (argc == 1) {
		/* -o or no option */
		s = query_loop(argv[0]);
		if (!s)
			bb_simple_perror_msg_and_die(argv[0]);
		printf("%s: %s\n", argv[0], s);
		if (ENABLE_FEATURE_CLEAN_UP)
			free(s);
		return EXIT_SUCCESS;
	}

	/* -o, -f or no option */
	while (1) {
		s = query_loop(dev);
		if (!s) {
			if (opt == 0x4) {
				puts(dev);
				return EXIT_SUCCESS;
			}
		} else {
			if (opt != 0x4)
				printf("%s: %s\n", dev, s);
			if (ENABLE_FEATURE_CLEAN_UP)
				free(s);
		}

		if (++dev[sizeof(dev) - 2] > '9')
			break;
	}
	return EXIT_SUCCESS;
}
