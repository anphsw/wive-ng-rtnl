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
/* orckit_quirks.h ...... fix quirks in orckit adsl modems
 *                        mulix <mulix@actcom.co.il>
 *
 * $Id: orckit_quirks.h,v 1.1.1.1 2008/07/21 09:19:28 james26_jang Exp $
 */

#ifndef INC_ORCKIT_QUIRKS_H_
#define INC_ORCKIT_QUIRKS_H_

#include "pptp_options.h"
#include "pptp_ctrl.h"
#include "pptp_msg.h"

/* return 0 on success, non zero otherwise */
int
orckit_atur3_build_hook(struct pptp_out_call_rqst* packt);

/* return 0 on success, non zero otherwise */
int
orckit_atur3_set_link_hook(struct pptp_set_link_info* packet,
			   int peer_call_id);

/* return 0 on success, non zero otherwise */
int
orckit_atur3_start_ctrl_conn_hook(struct pptp_start_ctrl_conn* packet);

#endif /* INC_ORCKIT_QUIRKS_H_ */
