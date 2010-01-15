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
#ifndef __MANAGEMENT_H__
#define __MANAGEMENT_H__

#define PROC_IF_STATISTIC	"/proc/net/dev"
#define PROC_MEM_STATISTIC	"/proc/meminfo"


#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3

#define WIFI_STATUS "iwpriv ra0 stat"

#define SYSTEM_COMMAND_LOG "/var/system_command.log"

void formDefineManagement(void);
void management_init(void);
char *setNthValueLong(int index, char *old_values, char *new_value);
#endif
