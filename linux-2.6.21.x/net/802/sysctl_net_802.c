/* -*- linux-c -*-
 *		sysctl_net_802.c: sysctl interface to net 802 subsystem.
 *
 *		Begun April 1, 1996, Mike Shaver.
 *		Added /proc/sys/net/802 directory entry (empty =) ). [MS]
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

#include <linux/mm.h>
#include <linux/if_tr.h>
#include <linux/sysctl.h>

struct ctl_table tr_table[] = {	{ 0 },};
