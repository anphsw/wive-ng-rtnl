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
 * File: e2fsbb.h
 *
 * Redefine a bunch of e2fsprogs stuff to use busybox routines
 * instead.  This makes upgrade between e2fsprogs versions easy.
 */

#ifndef __E2FSBB_H__
#define __E2FSBB_H__ 1

#include "libbb.h"

/* version we've last synced against */
#define E2FSPROGS_VERSION "1.38"
#define E2FSPROGS_DATE "30-Jun-2005"

typedef long errcode_t;
#define ERRCODE_RANGE 8
#define error_message(code) strerror((int) (code & ((1<<ERRCODE_RANGE)-1)))

/* header defines */
#define ENABLE_HTREE 1
#define HAVE_ERRNO_H 1
#define HAVE_EXT2_IOCTLS 1
#define HAVE_LINUX_FD_H 1
#define HAVE_MNTENT_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_NET_IF_H 1
#define HAVE_SYS_IOCTL_H 1
#define HAVE_SYS_MOUNT_H 1
#define HAVE_SYS_QUEUE_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1

/* Endianness */
#if BB_BIG_ENDIAN
#define ENABLE_SWAPFS 1
#define WORDS_BIGENDIAN 1
#endif

#endif /* __E2FSBB_H__ */
