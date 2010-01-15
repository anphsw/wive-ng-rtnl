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
#ifndef _IPTC_H_
	#define _IPTC_H_

void iptc_add_rule(const char *table,
                   const char *chain,
                   const char *protocol,
                   const char *iiface,
                   const char *oiface,
                   const char *src,
                   const char *dest,
                   const char *srcports,
                   const char *destports,
                   const char *target,
                   const char *dnat_to,
                   const int append);

void iptc_delete_rule(const char *table,
                      const char *chain,
                      const char *protocol,
                      const char *iniface,
                      const char *outiface,
                      const char *src,
                      const char *dest,
                      const char *srcports,
                      const char *destports,
                      const char *target,
                      const char *dnat_to);

#endif // _IPTC_H_
