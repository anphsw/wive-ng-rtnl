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
#ifndef _IPSECMAST_H
#define _IPSECMAST_H

#ifndef IP_IPSEC_REFINFO
#define IP_IPSEC_REFINFO 18
#endif

#ifndef IPSEC_SAREF_NULL
typedef uint32_t IPsecSAref_t;

#define IPSEC_SAREF_NULL ((IPsecSAref_t)0)
#endif

#endif
