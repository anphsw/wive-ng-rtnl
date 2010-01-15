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
 * newdir.c --- create a new directory block
 *
 * Copyright (C) 1994, 1995 Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 */

#include <stdio.h>
#include <string.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "ext2_fs.h"
#include "ext2fs.h"

#ifndef EXT2_FT_DIR
#define EXT2_FT_DIR		2
#endif

/*
 * Create new directory block
 */
errcode_t ext2fs_new_dir_block(ext2_filsys fs, ext2_ino_t dir_ino,
			       ext2_ino_t parent_ino, char **block)
{
	struct ext2_dir_entry	*dir = NULL;
	errcode_t		retval;
	char			*buf;
	int			rec_len;
	int			filetype = 0;

	EXT2_CHECK_MAGIC(fs, EXT2_ET_MAGIC_EXT2FS_FILSYS);

	retval = ext2fs_get_mem(fs->blocksize, &buf);
	if (retval)
		return retval;
	memset(buf, 0, fs->blocksize);
	dir = (struct ext2_dir_entry *) buf;
	dir->rec_len = fs->blocksize;

	if (dir_ino) {
		if (fs->super->s_feature_incompat &
		    EXT2_FEATURE_INCOMPAT_FILETYPE)
			filetype = EXT2_FT_DIR << 8;
		/*
		 * Set up entry for '.'
		 */
		dir->inode = dir_ino;
		dir->name_len = 1 | filetype;
		dir->name[0] = '.';
		rec_len = dir->rec_len - EXT2_DIR_REC_LEN(1);
		dir->rec_len = EXT2_DIR_REC_LEN(1);

		/*
		 * Set up entry for '..'
		 */
		dir = (struct ext2_dir_entry *) (buf + dir->rec_len);
		dir->rec_len = rec_len;
		dir->inode = parent_ino;
		dir->name_len = 2 | filetype;
		dir->name[0] = '.';
		dir->name[1] = '.';

	}
	*block = buf;
	return 0;
}
