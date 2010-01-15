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
#if !defined _NTP_IO_H
#define _NTP_IO_H
/*
 * POSIX says use <fnct.h> to get O_* symbols and 
 * SEEK_SET symbol form <unistd.h>.
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <stdio.h>
#ifdef HAVE_SYS_FILE_H
# include <sys/file.h>
#endif
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#if !defined(SEEK_SET) && defined(L_SET)
# define SEEK_SET L_SET
#endif

#ifdef SYS_WINNT
# include <io.h>
# include "win32_io.h"
#endif

/*
 * Define FNDELAY and FASYNC using O_NONBLOCK and O_ASYNC if we need
 * to (and can).  This is here initially for QNX, but may help for
 * others as well...
 */
#ifndef FNDELAY
# ifdef O_NONBLOCK
#  define FNDELAY O_NONBLOCK
# endif
#endif

#ifndef FASYNC
# ifdef O_ASYNC
#  define FASYNC O_ASYNC
# endif
#endif

#endif
