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
 * Small lzma deflate implementation.
 * Copyright (C) 2006  Aurelien Jacobs <aurel@gnuage.org>
 *
 * Licensed under GPL v2, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
#include "unarchive.h"

char FAST_FUNC get_header_tar_lzma(archive_handle_t *archive_handle)
{
	/* Can't lseek over pipes */
	archive_handle->seek = seek_by_read;

	open_transformer(archive_handle->src_fd, unpack_lzma_stream, "unlzma");
	archive_handle->offset = 0;
	while (get_header_tar(archive_handle) == EXIT_SUCCESS)
		continue;

	/* Can only do one file at a time */
	return EXIT_FAILURE;
}
