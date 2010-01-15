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
    See the file COPYING.LIB
*/

#include "fuse.h"

struct fuse_session;
struct fuse_chan;
struct fuse_lowlevel_ops;
struct fuse_req;

struct fuse_cmd {
    char *buf;
    size_t buflen;
    struct fuse_chan *ch;
};

struct fuse_chan *fuse_kern_chan_new(int fd);

void fuse_kern_unmount(const char *mountpoint, int fd);
int fuse_kern_mount(const char *mountpoint, struct fuse_args *args);
