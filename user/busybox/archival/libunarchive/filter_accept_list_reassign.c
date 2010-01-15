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
 *  Copyright (C) 2002 by Glenn McGrath
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
#include "unarchive.h"

/* Built and used only if ENABLE_DPKG || ENABLE_DPKG_DEB */

/*
 * Reassign the subarchive metadata parser based on the filename extension
 * e.g. if its a .tar.gz modify archive_handle->sub_archive to process a .tar.gz
 * or if its a .tar.bz2 make archive_handle->sub_archive handle that
 */
char FAST_FUNC filter_accept_list_reassign(archive_handle_t *archive_handle)
{
	/* Check the file entry is in the accept list */
	if (find_list_entry(archive_handle->accept, archive_handle->file_header->name)) {
		const char *name_ptr;

		/* Find extension */
		name_ptr = strrchr(archive_handle->file_header->name, '.');
		if (!name_ptr)
			return EXIT_FAILURE;
		name_ptr++;

		/* Modify the subarchive handler based on the extension */
		if (ENABLE_FEATURE_SEAMLESS_GZ
		 && strcmp(name_ptr, "gz") == 0
		) {
			archive_handle->action_data_subarchive = get_header_tar_gz;
			return EXIT_SUCCESS;
		}
		if (ENABLE_FEATURE_SEAMLESS_BZ2
		 && strcmp(name_ptr, "bz2") == 0
		) {
			archive_handle->action_data_subarchive = get_header_tar_bz2;
			return EXIT_SUCCESS;
		}
		if (ENABLE_FEATURE_SEAMLESS_LZMA
		 && strcmp(name_ptr, "lzma") == 0
		) {
			archive_handle->action_data_subarchive = get_header_tar_lzma;
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}
