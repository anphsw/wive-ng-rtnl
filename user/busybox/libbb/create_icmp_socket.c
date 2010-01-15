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
 * create raw socket for icmp protocol
 * and drop root privileges if running setuid
 */

#include "libbb.h"

int FAST_FUNC create_icmp_socket(void)
{
	int sock;
#if 0
	struct protoent *proto;
	proto = getprotobyname("icmp");
	/* if getprotobyname failed, just silently force
	 * proto->p_proto to have the correct value for "icmp" */
	sock = socket(AF_INET, SOCK_RAW,
			(proto ? proto->p_proto : 1)); /* 1 == ICMP */
#else
	sock = socket(AF_INET, SOCK_RAW, 1); /* 1 == ICMP */
#endif
	if (sock < 0) {
		if (errno == EPERM)
			bb_error_msg_and_die(bb_msg_perm_denied_are_you_root);
		bb_perror_msg_and_die(bb_msg_can_not_create_raw_socket);
	}

	/* drop root privs if running setuid */
	xsetuid(getuid());

	return sock;
}
