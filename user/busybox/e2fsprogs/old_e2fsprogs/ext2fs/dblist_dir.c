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
 * dblist_dir.c --- iterate by directory entry
 *
 * Copyright 1997 by Theodore Ts'o
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Public
 * License.
 * %End-Header%
 *
 */

#include <stdio.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <time.h>

#include "ext2_fs.h"
#include "ext2fsP.h"

static int db_dir_proc(ext2_filsys fs, struct ext2_db_entry *db_info,
		       void *priv_data);

errcode_t ext2fs_dblist_dir_iterate(ext2_dblist dblist,
				    int	flags,
				    char	*block_buf,
				    int (*func)(ext2_ino_t dir,
						int	entry,
						struct ext2_dir_entry *dirent,
						int	offset,
						int	blocksize,
						char	*buf,
						void	*priv_data),
				    void *priv_data)
{
	errcode_t		retval;
	struct dir_context	ctx;

	EXT2_CHECK_MAGIC(dblist, EXT2_ET_MAGIC_DBLIST);

	ctx.dir = 0;
	ctx.flags = flags;
	if (block_buf)
		ctx.buf = block_buf;
	else {
		retval = ext2fs_get_mem(dblist->fs->blocksize, &ctx.buf);
		if (retval)
			return retval;
	}
	ctx.func = func;
	ctx.priv_data = priv_data;
	ctx.errcode = 0;

	retval = ext2fs_dblist_iterate(dblist, db_dir_proc, &ctx);

	if (!block_buf)
		ext2fs_free_mem(&ctx.buf);
	if (retval)
		return retval;
	return ctx.errcode;
}

static int db_dir_proc(ext2_filsys fs, struct ext2_db_entry *db_info,
		       void *priv_data)
{
	struct dir_context	*ctx;

	ctx = (struct dir_context *) priv_data;
	ctx->dir = db_info->ino;

	return ext2fs_process_dir_block(fs, &db_info->blk,
					db_info->blockcnt, 0, 0, priv_data);
}
