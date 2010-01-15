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
 * Utility routines.
 *
 * Copyright (C) 2007 by Denys Vlasenko <vda.linux@googlemail.com>
 *
 * Licensed under GPLv2, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

/* Wrapper which restarts poll on EINTR or ENOMEM.
 * On other errors does perror("poll") and returns.
 * Warning! May take longer than timeout_ms to return! */
int FAST_FUNC safe_poll(struct pollfd *ufds, nfds_t nfds, int timeout)
{
	while (1) {
		int n = poll(ufds, nfds, timeout);
		if (n >= 0)
			return n;
		/* Make sure we inch towards completion */
		if (timeout > 0)
			timeout--;
		/* E.g. strace causes poll to return this */
		if (errno == EINTR)
			continue;
		/* Kernel is very low on memory. Retry. */
		/* I doubt many callers would handle this correctly! */
		if (errno == ENOMEM)
			continue;
		bb_perror_msg("poll");
		return n;
	}
}
