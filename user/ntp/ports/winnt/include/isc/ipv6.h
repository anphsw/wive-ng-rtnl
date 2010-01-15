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
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2000-2002  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: ipv6.h,v 1.1.1.1 2007-04-02 09:43:49 yy Exp $ */

#ifndef ISC_IPV6_H
#define ISC_IPV6_H 1

/*****
 ***** Module Info
 *****/

/*
 * IPv6 definitions for systems which do not support IPv6.
 *
 * MP:
 *	No impact.
 *
 * Reliability:
 *	No anticipated impact.
 *
 * Resources:
 *	N/A.
 *
 * Security:
 *	No anticipated impact.
 *
 * Standards:
 *	RFC 2553.
 */

#if _MSC_VER < 1300
#define in6_addr in_addr6
#endif

#ifndef IN6ADDR_ANY_INIT
#define IN6ADDR_ANY_INIT 	{{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }}
#endif
#ifndef IN6ADDR_LOOPBACK_INIT
#define IN6ADDR_LOOPBACK_INIT 	{{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }}
#endif

extern const struct in6_addr isc_in6addr_any;
extern const struct in6_addr isc_in6addr_loopback;

/*
 * Unspecified
 */

#ifndef IN6_IS_ADDR_UNSPECIFIED
#define IN6_IS_ADDR_UNSPECIFIED(a)     (\
*((u_long *)((a)->s6_addr)    ) == 0 && \
*((u_long *)((a)->s6_addr) + 1) == 0 && \
*((u_long *)((a)->s6_addr) + 2) == 0 && \
*((u_long *)((a)->s6_addr) + 3) == 0 \
)
#endif
/*
 * Loopback
 */
#ifndef IN6_IS_ADDR_LOOPBACK
#define IN6_IS_ADDR_LOOPBACK(a) (\
*((u_long *)((a)->s6_addr)    ) == 0 && \
*((u_long *)((a)->s6_addr) + 1) == 0 && \
*((u_long *)((a)->s6_addr) + 2) == 0 && \
*((u_long *)((a)->s6_addr) + 3) == htonl(1) \
)
#endif

/*
 * IPv4 compatible
 */
#ifndef IN6_IS_ADDR_V4COMPAT
#define IN6_IS_ADDR_V4COMPAT(a)  (\
*((u_long *)((a)->s6_addr)    ) == 0 && \
*((u_long *)((a)->s6_addr) + 1) == 0 && \
*((u_long *)((a)->s6_addr) + 2) == 0 && \
*((u_long *)((a)->s6_addr) + 3) != 0 && \
*((u_long *)((a)->s6_addr) + 3) != htonl(1) \
)
#endif

/*
 * Mapped
 */
#ifndef IN6_IS_ADDR_V4MAPPED
#define IN6_IS_ADDR_V4MAPPED(a) (\
*((u_long *)((a)->s6_addr)    ) == 0 && \
*((u_long *)((a)->s6_addr) + 1) == 0 && \
*((u_long *)((a)->s6_addr) + 2) == htonl(0x0000ffff))
#endif

/*
 * Multicast
 */
#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a)	\
	((a)->s6_addr[0] == 0xffU)
#endif
/*
 * Unicast link / site local.
 */
#ifndef IN6_IS_ADDR_LINKLOCAL
#define IN6_IS_ADDR_LINKLOCAL(a)	(\
(*((u_long *)((a)->s6_addr)    ) == 0xfe) && \
((*((u_long *)((a)->s6_addr) + 1) & 0xc0) == 0x80))
#endif

#ifndef IN6_IS_ADDR_SITELOCAL
#define IN6_IS_ADDR_SITELOCAL(a)	(\
(*((u_long *)((a)->s6_addr)    ) == 0xfe) && \
((*((u_long *)((a)->s6_addr) + 1) & 0xc0) == 0xc0))
#endif

#endif /* ISC_IPV6_H */
