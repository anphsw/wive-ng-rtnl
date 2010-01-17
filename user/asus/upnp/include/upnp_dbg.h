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
 * Minimal debug/trace/assert driver definitions for
 * Broadcom UPNP implementation
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: upnp_dbg.h,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#ifndef _upnp_dbg_
#define _upnp_dbg_

#define	UPNP_MAPPING(args)
#define	UPNP_ERROR(args)
#define	UPNP_TRACE(args) 
#define	UPNP_PRHDRS(i, s, h, p, n, l)
#define	UPNP_PRPKT(m, b, n)
#define	UPNP_INFORM(args)
#define UPNP_TRACE_ACTION(svc, ac) 
#define	UPNP_HTTP(args)
#define	UPNP_SOCKET(args) 

#define UPNP_MAPPING_ON()	0
#define UPNP_RESPONSE_ON()	0
#define UPNP_ERROR_ON()		0
#define UPNP_PRHDRS_ON()	0
#define UPNP_PRPKT_ON()		0
#define UPNP_INFORM_ON()	0
#define UPNP_PRINTRX_ON()	0
#define UPNP_PRINTTX_ON()	0
#define UPNP_HTTP_TRACE_ON()	0
#define UPNP_HTTP_HDRS_ON()	0

#define	UPNP_PREVENT(args)
#define	UPNP_SUBSCRIBE(args)

#define UPNP_ACTION(psvc, ac, args, nargs)
#define UPNP_RESPONSE(ns, ac, args, nargs)
#define UPNP_TRACE_ACTION_ON()	0
#define UPNP_DUMP_ACTION_ON()	0




#endif /* _upnp_dbg_ */
