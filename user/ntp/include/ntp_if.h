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
 * Sockets are not standard.
 * So hide uglyness in include file.
 */
/* was: defined(SYS_CONVEXOS9) */
#if defined(HAVE__SYS_SYNC_QUEUE_H) && defined(HAVE__SYS_SYNC_SEMA_H)
# include "/sys/sync/queue.h"
# include "/sys/sync/sema.h"
#endif

/* was: (defined(SYS_SOLARIS) && !defined(bsd)) || defined(SYS_SUNOS4) */
/* was: defined(SYS_UNIXWARE1) */
#ifdef HAVE_SYS_SOCKIO_H
# include <sys/sockio.h>
#endif

/* was: #if defined(SYS_PTX) || defined(SYS_SINIXM) */
#ifdef HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif
#ifdef HAVE_SYS_STROPTS_H
# include <sys/stropts.h>
#endif

#ifdef HAVE_NET_IF_H
# include <net/if.h>
#endif /* HAVE_NET_IF_H */
