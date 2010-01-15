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
 *	wireless.h -- Wireless Configuration Header 
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: wireless.h,v 1.9 2008-01-25 06:27:28 yy Exp $
 */

void formDefineWireless(void);
void restart8021XDaemon(int nvram);
void updateFlash8021x(int nvram);
void Security(int nvram, webs_t wp, char_t *path, char_t *query);
void confWPAGeneral(int nvram, webs_t wp, int mbssid);
void confWEP(int nvram, webs_t wp, int mbssid);
void conf8021x(int nvram, webs_t wp, int mbssid);
void getSecurity(int nvram, webs_t wp, char_t *path, char_t *query);
void DeleteAccessPolicyList(int nvram, webs_t wp, char_t *path, char_t *query);
