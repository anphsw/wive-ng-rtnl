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
 * $Id: etherlink.c,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"

extern void soap_response(int, const char *, const char *,  pvar_entry_t, int);
extern int DefaultAction(int, PService, PAction ac, pvar_entry_t, int);

#define GetEthernetLinkStatus DefaultAction

char *EthernetLinkStatus_allowedValueList[] = {  "Up", "Down", "Unavailable", 0 };

static VarTemplate StateVariables[] = { 
    { "EthernetLinkStatus", "", VAR_EVENTED|VAR_STRING|VAR_LIST,  (allowedValue) { EthernetLinkStatus_allowedValueList } }, 
    { 0 } 
};

#define VAR_EthernetLinkStatus	0

static Action _GetEthernetLinkStatus = { 
    "GetEthernetLinkStatus", GetEthernetLinkStatus,
        {
            {"NewEthernetLinkStatus", VAR_EthernetLinkStatus, VAR_OUT},
            { 0 }
        }
};

static PAction Actions[] = {
    &_GetEthernetLinkStatus,
    NULL
};

ServiceTemplate Template_WANEthernetLinkConfig = {
    "WANEthernetLinkConfig:1",
    NULL,
    NULL,
    NULL,   /* SVCXML */
    ARRAYSIZE(StateVariables)-1, StateVariables,
    Actions
};





