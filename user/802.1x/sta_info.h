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
#ifndef STA_INFO_H
#define STA_INFO_H

struct sta_info* Ap_get_sta(rtapd *apd, u8 *sa, u8 *apidx, u16 ethertype, int stop);
struct sta_info* Ap_get_sta_radius_identifier(rtapd *apd, u8 radius_identifier);
void Ap_sta_hash_add(rtapd *apd, struct sta_info *sta);
void Ap_free_sta(rtapd *apd, struct sta_info *sta);
void Apd_free_stas(rtapd *apd);
void Ap_sta_session_timeout(rtapd *apd, struct sta_info *sta, u32 session_timeout);
void Ap_sta_no_session_timeout(rtapd *apd, struct sta_info *sta);

#endif /* STA_INFO_H */
