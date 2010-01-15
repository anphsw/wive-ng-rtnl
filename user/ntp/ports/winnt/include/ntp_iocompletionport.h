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
#if !defined __ntp_iocompletionport_h
# define __ntp_iocompletionport_h

#include "ntp.h"

# if defined(HAVE_IO_COMPLETION_PORT)

struct refclockio;


extern	void	init_io_completion_port (void);
extern	void	uninit_io_completion_port (void);

extern	int	io_completion_port_add_clock_io	(struct refclockio * /*rio */);

extern	int	io_completion_port_add_socket (SOCKET fd, struct interface *);

extern	DWORD	io_completion_port_sendto (struct interface *, struct pkt *, int, struct sockaddr_storage*);

extern	HANDLE get_io_event (void);

int GetReceivedBuffers(void);

# endif

#endif
