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
 *  $Id: lanhostconfig.c,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "lanhostconfig.h"
#include "bcmnvram.h"

int LANHostConfig_GetVar(struct Service *psvc, int varindex)
{
    struct StateVar *var;

    var = &(psvc->vars[varindex]);

    switch (varindex) {

    case VAR_DomainName: 
	strcpy(var->value, nvram_safe_get("lan_domain"));
	break;

    case VAR_SubnetMask:
	strcpy(var->value, nvram_safe_get("lan_netmask"));
	break;

    case VAR_MinAddress:
	strcpy(var->value, nvram_safe_get("dhcp_start"));
	break;

    case VAR_MaxAddress:
	strcpy(var->value, nvram_safe_get("dhcp_end"));
	break;
    } /* end-switch */

    return TRUE;
}


int LANHostConfig_SetDomainName(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
//       {"NewDomainName", VAR_DomainName, VAR_IN},
{
    uint success = TRUE; /* assume no error will occur */

    nvram_set("lan_domain", ac->params[0].value);
    nvram_commit();
    // restart ?? 
    
    return success;
}
