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
 * Utility routines.
 *
 * Based in part on code from sash, Copyright (c) 1999 by David I. Bell
 * Permission has been granted to redistribute this code under the GPL.
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include <sys/stat.h>
#include "libbb.h"

/*
 * Return TRUE if fileName is a directory.
 * Nonexistent files return FALSE.
 */
int FAST_FUNC is_directory(const char *fileName, const int followLinks, struct stat *statBuf)
{
	int status;
	struct stat astatBuf;

	if (statBuf == NULL) {
		/* use auto stack buffer */
		statBuf = &astatBuf;
	}

	if (followLinks)
		status = stat(fileName, statBuf);
	else
		status = lstat(fileName, statBuf);

	status = (status == 0 && S_ISDIR(statBuf->st_mode));

	return status;
}
