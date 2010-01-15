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
 * Not all machines define FD_SET in sys/types.h
 */ 
#ifndef _ntp_select_h
#define _ntp_select_h

/* Was: (defined(RS6000)||defined(SYS_PTX))&&!defined(_BSD) */
/* Could say: !defined(FD_SET) && defined(HAVE_SYS_SELECT_H) */
#if defined(HAVE_SYS_SELECT_H) && !defined(_BSD)
#ifndef SYS_VXWORKS
#include <sys/select.h>
#else
#include <sockLib.h>
extern int  select P((int width, fd_set *pReadFds, fd_set *pWriteFds,
            fd_set *pExceptFds, struct timeval *pTimeOut));

#endif
#endif

#if !defined(FD_SET) && !defined(SYS_WINNT)
#define NFDBITS         32
#define FD_SETSIZE      32
#define FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      memset((char *)(p), 0, sizeof(*(p)))
#endif

#if defined(VMS)
typedef struct {
    unsigned int fds_bits[1];
} fd_set;
#endif

#endif /* _ntp_select_h */
