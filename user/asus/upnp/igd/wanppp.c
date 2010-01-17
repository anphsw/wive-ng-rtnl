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
 *  $Id: wanppp.c,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#include "upnp_dbg.h"
#include "upnp_osl.h"
#include "upnp.h"
#include "igd.h"
#include "wanppp.h"
#include "bcmnvram.h"

#if defined(linux)
#define    pppoe_up()
#define    pppoe_down()
#else
extern void pppoe_up();
extern void pppoe_down();
#endif


extern void igd_restart(int secs);
extern char * safe_snprintf(char *str, int *len, const char *fmt, ...);

extern char *WANPPPConnection_PossibleConnectionTypes_allowedValueList[];

int WANPPPConnection_GetVar(struct Service *psvc, int varindex)
{
    struct StateVar *var;
    char **p, *cur;
    int len;

    var = &(psvc->vars[varindex]);

    switch (varindex) {
    case VAR_PossibleConnectionTypes:
	cur = var->value;
	len = sizeof(var->value);
	for (p = WANPPPConnection_PossibleConnectionTypes_allowedValueList; *p; p++) {
	    if (cur != var->value)
		cur = safe_snprintf(cur, &len, ",");
	    cur = safe_snprintf(cur, &len, "%s", *p);
	}
	break;
    case VAR_UserName:
	strcpy(var->value, nvram_safe_get("wan_pppoe_username"));
	break;
    case VAR_Password:
	strcpy(var->value, nvram_safe_get("wan_pppoe_passwd"));
	break;
    case VAR_IdleDisconnectTime:
	strcpy(var->value, nvram_safe_get("wan_pppoe_idletime"));
	break;
    case VAR_ExternalIPAddress: 
	strcpy(var->value, nvram_safe_get("wan_ipaddr_t"));
	break;
    }
    return TRUE;
}


int WANPPPConnection_ConfigureConnection(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
//       {"NewUserName", VAR_UserName, VAR_IN},
//       {"NewPassword", VAR_Password, VAR_IN},
{
    uint success = TRUE; /* assume no error will occur */

    if (strlen(ac->params[0].value) > 32 || strlen(ac->params[1].value) > 32) {
	soap_error( uclient, SOAP_INVALID_ARGS);
	success = FALSE;
    } else {
	nvram_set("wan_pppoe_username", ac->params[0].value);
	nvram_set("wan_pppoe_passwd", ac->params[1].value);
	nvram_commit();
    }

    return success;
}


int WANPPPConnection_SetIdleDisconnectTime(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
//       {"NewIdleDisconnectTime", VAR_IdleDisconnectTime, VAR_IN},
{
    uint success = TRUE; /* assume no error will occur */

    nvram_set("wan_pppoe_idletime", ac->params[0].value);
    nvram_commit();

    return success;
}


int WANPPPConnection_RequestConnection(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
    char tmp[100];

    /* should probably confirm that the pppoe_username and ppoe_passwd
       are set here. */
    nvram_set(igd_pri_wan_var(tmp, sizeof(tmp), "proto"), "pppoe");

    printf("PPP_CONNECTING %s\n", tmp);

    nvram_commit();
    pppoe_up();
    
    return TRUE;
}

int WANPPPConnection_ForceTermination(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
{
    pppoe_down();
    
    return TRUE;
}


int WANPPPConnection_SetConnectionType(UFILE *uclient, PService psvc, PAction ac, pvar_entry_t args, int nargs)
/*     "SetConnectionType", WANPPPConnection_SetConnectionType, */
{
    snprintf(psvc->vars[VAR_ConnectionType].value, sizeof(psvc->vars[VAR_ConnectionType].value), ac->params[0].value);

    return TRUE;
}
