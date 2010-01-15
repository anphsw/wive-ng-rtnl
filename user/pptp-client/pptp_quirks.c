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
/* pptp_quirks.c ...... various options to fix quirks found in buggy adsl modems
 *                      mulix <mulix@actcom.co.il>
 *
 * $Id: pptp_quirks.c,v 1.1.1.1 2008/07/21 09:19:28 james26_jang Exp $
 */

#include <string.h>
#include "orckit_quirks.h"
#include "pptp_quirks.h"

static int quirk_index = -1;

struct pptp_fixup pptp_fixups[] = {
    {BEZEQ_ISRAEL, ORCKIT, ORCKIT_ATUR3,
     orckit_atur3_build_hook,
     orckit_atur3_start_ctrl_conn_hook,
     orckit_atur3_set_link_hook}
};

static int fixups_sz = sizeof(pptp_fixups)/sizeof(pptp_fixups[0]);

/* return 0 on success, non 0 otherwise */
int set_quirk_index(int index)
{
    if (index >= 0 && index < fixups_sz) {
	quirk_index = index;
	return 0;
    }

    return -1;
}

int get_quirk_index()
{
    return quirk_index;
}

/* return the index for this isp in the quirks table, -1 if not found */
int find_quirk(const char* isp_name)
{
    int i = 0;
    if (isp_name) {
	while (i < fixups_sz && pptp_fixups[i].isp) {
	    if (!strcmp(pptp_fixups[i].isp, isp_name)) {
		return i;
	    }
	    ++i;
	}
    }

    return -1;
}


