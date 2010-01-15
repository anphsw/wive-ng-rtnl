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
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"
#include "unarchive.h"

void FAST_FUNC header_verbose_list(const file_header_t *file_header)
{
	struct tm *mtime = localtime(&(file_header->mtime));

#if ENABLE_FEATURE_TAR_UNAME_GNAME
	char uid[8];
	char gid[8];
	char *user = file_header->uname;
	char *group = file_header->gname;

	if (user == NULL) {
		snprintf(uid, sizeof(uid), "%u", (unsigned)file_header->uid);
		user = uid;
	}
	if (group == NULL) {
		snprintf(gid, sizeof(gid), "%u", (unsigned)file_header->gid);
		group = gid;
	}
	printf("%s %s/%s %9u %4u-%02u-%02u %02u:%02u:%02u %s",
		bb_mode_string(file_header->mode),
		user,
		group,
		(unsigned int) file_header->size,
		1900 + mtime->tm_year,
		1 + mtime->tm_mon,
		mtime->tm_mday,
		mtime->tm_hour,
		mtime->tm_min,
		mtime->tm_sec,
		file_header->name);
#else /* !FEATURE_TAR_UNAME_GNAME */
	printf("%s %d/%d %9"OFF_FMT"u %4u-%02u-%02u %02u:%02u:%02u %s",
		bb_mode_string(file_header->mode),
		file_header->uid,
		file_header->gid,
		file_header->size,
		1900 + mtime->tm_year,
		1 + mtime->tm_mon,
		mtime->tm_mday,
		mtime->tm_hour,
		mtime->tm_min,
		mtime->tm_sec,
		file_header->name);
#endif /* FEATURE_TAR_UNAME_GNAME */

	if (file_header->link_target) {
		printf(" -> %s", file_header->link_target);
	}
	bb_putchar('\n');
}
