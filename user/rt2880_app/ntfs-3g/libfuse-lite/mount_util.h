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
/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU LGPLv2.
    See the file COPYING.LIB.
*/

#include <sys/types.h>

int fuse_mnt_add_mount(const char *progname, const char *fsname,
                       const char *mnt, const char *type, const char *opts);
int fuse_mnt_umount(const char *progname, const char *mnt, int lazy);
char *fuse_mnt_resolve_path(const char *progname, const char *orig);
int fuse_mnt_check_fuseblk(void);

int fusermount(int unmount, int quiet, int lazy, const char *opts, 
	       const char *origmnt);

