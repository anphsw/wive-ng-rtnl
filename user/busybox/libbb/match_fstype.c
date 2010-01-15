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
 * Match fstypes for use in mount unmount
 * We accept notmpfs,nfs but not notmpfs,nonfs
 * This allows us to match fstypes that start with no like so
 *   mount -at ,noddy
 *
 * Returns 0 for a match, otherwise -1
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

int FAST_FUNC match_fstype(const struct mntent *mt, const char *fstype)
{
	int no = 0;
	int len;

	if (!mt)
		return -1;

	if (!fstype)
		return 0;

	if (fstype[0] == 'n' && fstype[1] == 'o') {
		no = -1;
		fstype += 2;
	}

	len = strlen(mt->mnt_type);
	while (fstype) {
		if (!strncmp(mt->mnt_type, fstype, len)
		 && (!fstype[len] || fstype[len] == ',')
		) {
			return no;
		}
		fstype = strchr(fstype, ',');
		if (fstype)
			fstype++;
	}

	return -(no + 1);
}
