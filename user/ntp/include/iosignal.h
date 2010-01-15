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
#if !defined _ntp_iosignaled_h
#define _ntp_iosignaled_h

#include "ntp_refclock.h"

#if defined(HAVE_SIGNALED_IO)
extern void			block_sigio     P((void));
extern void			unblock_sigio   P((void));
extern int			init_clock_sig	P((struct refclockio *));
extern void			init_socket_sig P((int));
extern void			set_signal		P((void));
RETSIGTYPE	sigio_handler	P((int));

# define BLOCKIO()	 ((void) block_sigio())
# define UNBLOCKIO() ((void) unblock_sigio())

#else

# define BLOCKIO()
# define UNBLOCKIO()
#endif /* HAVE_SIGNALED_IO */

#endif
