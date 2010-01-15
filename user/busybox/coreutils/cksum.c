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
 * cksum - calculate the CRC32 checksum of a file
 *
 * Copyright (C) 2006 by Rob Sullivan, with ideas from code by Walter Harms
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details. */

#include "libbb.h"

int cksum_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int cksum_main(int argc UNUSED_PARAM, char **argv)
{
	uint32_t *crc32_table = crc32_filltable(NULL, 1);
	uint32_t crc;
	long length, filesize;
	int bytes_read;
	uint8_t *cp;

#if ENABLE_DESKTOP
	getopt32(argv, ""); /* coreutils 6.9 compat */
	argv += optind;
#else
	argv++;
#endif

	do {
		int fd = open_or_warn_stdin(*argv ? *argv : bb_msg_standard_input);

		if (fd < 0)
			continue;
		crc = 0;
		length = 0;

#define read_buf bb_common_bufsiz1
		while ((bytes_read = safe_read(fd, read_buf, sizeof(read_buf))) > 0) {
			cp = (uint8_t *) read_buf;
			length += bytes_read;
			do {
				crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *cp++];
			} while (--bytes_read);
		}
		close(fd);

		filesize = length;

		for (; length; length >>= 8)
			crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ length) & 0xff];
		crc ^= 0xffffffffL;

		printf((*argv ? "%" PRIu32 " %li %s\n" : "%" PRIu32 " %li\n"),
				crc, filesize, *argv);
	} while (*argv && *++argv);

	fflush_stdout_and_exit(EXIT_SUCCESS);
}
