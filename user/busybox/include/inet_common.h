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
 * stolen from net-tools-1.59 and stripped down for busybox by
 *                      Erik Andersen <andersen@codepoet.org>
 *
 * Heavily modified by Manuel Novoa III       Mar 12, 2001
 *
 */

#include "platform.h"

/* hostfirst!=0 If we expect this to be a hostname,
   try hostname database first
 */
int INET_resolve(const char *name, struct sockaddr_in *s_in, int hostfirst) FAST_FUNC;

/* numeric: & 0x8000: "default" instead of "*",
 *          & 0x4000: host instead of net,
 *          & 0x0fff: don't resolve
 */

int INET6_resolve(const char *name, struct sockaddr_in6 *sin6) FAST_FUNC;

/* These return malloced string */
char *INET_rresolve(struct sockaddr_in *s_in, int numeric, uint32_t netmask) FAST_FUNC;
char *INET6_rresolve(struct sockaddr_in6 *sin6, int numeric) FAST_FUNC;
