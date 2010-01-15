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
 * Mini loadkmap implementation for busybox
 *
 * Copyright (C) 1998 Enrique Zanardi <ezanardi@ull.es>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 *
 */

#include "libbb.h"

#define BINARY_KEYMAP_MAGIC "bkeymap"

/* From <linux/kd.h> */
struct kbentry {
	unsigned char kb_table;
	unsigned char kb_index;
	unsigned short kb_value;
};
/* sets one entry in translation table */
#define KDSKBENT        0x4B47

/* From <linux/keyboard.h> */
#define NR_KEYS         128
#define MAX_NR_KEYMAPS  256

int loadkmap_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int loadkmap_main(int argc UNUSED_PARAM, char **argv UNUSED_PARAM)
{
	struct kbentry ke;
	int i, j, fd;
	uint16_t ibuff[NR_KEYS];
	RESERVE_CONFIG_BUFFER(flags,MAX_NR_KEYMAPS);

/*  bb_warn_ignoring_args(argc>=2);*/

	fd = get_console_fd_or_die();

	xread(STDIN_FILENO, flags, 7);
	if (strncmp(flags, BINARY_KEYMAP_MAGIC, 7))
		bb_error_msg_and_die("not a valid binary keymap");

	xread(STDIN_FILENO, flags, MAX_NR_KEYMAPS);

	for (i = 0; i < MAX_NR_KEYMAPS; i++) {
		if (flags[i] == 1) {
			xread(STDIN_FILENO, ibuff, NR_KEYS * sizeof(uint16_t));
			for (j = 0; j < NR_KEYS; j++) {
				ke.kb_index = j;
				ke.kb_table = i;
				ke.kb_value = ibuff[j];
				ioctl(fd, KDSKBENT, &ke);
			}
		}
	}

	if (ENABLE_FEATURE_CLEAN_UP) {
		close(fd);
		RELEASE_CONFIG_BUFFER(flags);
	}
	return EXIT_SUCCESS;
}
