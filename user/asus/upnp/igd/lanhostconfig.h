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
 *  Copyright 2004, Broadcom Corporation      
 *  All Rights Reserved.      
 *        
 *  THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY      
 *  KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM      
 *  SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS      
 *  FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.      
 *
 *  $Id: lanhostconfig.h,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#if !defined(_lanhostconfig_h_)
#define _lanhostconfig_h_

#define VAR_DHCPServerConfigurable		0
#define VAR_DHCPRelay		1
#define VAR_SubnetMask		2
#define VAR_IPRouters		3
#define VAR_DNSServers		4
#define VAR_DomainName		5
#define VAR_MinAddress		6
#define VAR_MaxAddress		7
#define VAR_ReservedAddresses		8

int LANHostConfig_GetVar(struct Service *psvc, int varindex);
int LANHostConfig_SetDomainName(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs);


#endif /* _lanhostconfig_h_ */
