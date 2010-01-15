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
 * lsattr.c		- List file attributes on an ext2 file system
 *
 * Copyright (C) 1993, 1994  Remy Card <card@masi.ibp.fr>
 *                           Laboratoire MASI, Institut Blaise Pascal
 *                           Universite Pierre et Marie Curie (Paris VI)
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

/*
 * History:
 * 93/10/30	- Creation
 * 93/11/13	- Replace stat() calls by lstat() to avoid loops
 * 94/02/27	- Integrated in Ted's distribution
 * 98/12/29	- Display version info only when -V specified (G M Sipe)
 */

#include "libbb.h"
#include "e2fs_lib.h"

enum {
	OPT_RECUR      = 0x1,
	OPT_ALL        = 0x2,
	OPT_DIRS_OPT   = 0x4,
	OPT_PF_LONG    = 0x8,
	OPT_GENERATION = 0x10,
};

static void list_attributes(const char *name)
{
	unsigned long fsflags;
	unsigned long generation;

	if (fgetflags(name, &fsflags) != 0)
		goto read_err;

	if (option_mask32 & OPT_GENERATION) {
		if (fgetversion(name, &generation) != 0)
			goto read_err;
		printf("%5lu ", generation);
	}

	if (option_mask32 & OPT_PF_LONG) {
		printf("%-28s ", name);
		print_e2flags(stdout, fsflags, PFOPT_LONG);
		bb_putchar('\n');
	} else {
		print_e2flags(stdout, fsflags, 0);
		printf(" %s\n", name);
	}

	return;
 read_err:
	bb_perror_msg("reading %s", name);
}

static int lsattr_dir_proc(const char *dir_name, struct dirent *de,
			   void *private UNUSED_PARAM)
{
	struct stat st;
	char *path;

	path = concat_path_file(dir_name, de->d_name);

	if (lstat(path, &st) != 0)
		bb_perror_msg("stat %s", path);
	else if (de->d_name[0] != '.' || (option_mask32 & OPT_ALL)) {
		list_attributes(path);
		if (S_ISDIR(st.st_mode) && (option_mask32 & OPT_RECUR)
		 && !DOT_OR_DOTDOT(de->d_name)
		) {
			printf("\n%s:\n", path);
			iterate_on_dir(path, lsattr_dir_proc, NULL);
			bb_putchar('\n');
		}
	}

	free(path);
	return 0;
}

static void lsattr_args(const char *name)
{
	struct stat st;

	if (lstat(name, &st) == -1) {
		bb_perror_msg("stat %s", name);
	} else if (S_ISDIR(st.st_mode) && !(option_mask32 & OPT_DIRS_OPT)) {
		iterate_on_dir(name, lsattr_dir_proc, NULL);
	} else {
		list_attributes(name);
	}
}

int lsattr_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int lsattr_main(int argc UNUSED_PARAM, char **argv)
{
	getopt32(argv, "Radlv");
	argv += optind;

	if (!*argv)
		*--argv = (char*)".";
	do lsattr_args(*argv++); while (*argv);

	return EXIT_SUCCESS;
}
