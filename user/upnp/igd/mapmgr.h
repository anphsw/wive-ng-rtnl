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
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: mapmgr.h,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#ifndef _mapmgr_h_
#define _mapmgr_h_

typedef struct _netconf_nat_t mapping_t;

void mapmgr_update();
bool mapmgr_get_port_map(int n, mapping_t *m);
bool mapmgr_add_port_map(mapping_t *m);
bool mapmgr_delete_port_map(int n);
bool mapmgr_get_range_map(int n, mapping_t *m);
bool mapmgr_add_range_map(mapping_t *m);
bool mapmgr_delete_range_map(int n);
int mapmgr_port_map_count();
int mapmgr_range_map_count();

#define MAX_PORT_MAPPINGS FD_SETSIZE


#endif /* _mapmgr_h_ */
