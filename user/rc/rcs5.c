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
#include <stdio.h>

extern int start_switch(void);
extern int start_vlan(void);
extern int stop_vlan(void);
#ifdef CONFIG_PPTP_CLIENT
extern int start_pptp();
extern int stop_pptp();
#endif
#ifdef CONFIG_IPSEC_FREESWAN
extern int start_ipsec();
extern int stop_ipsec();
#endif

void extra_restore_defaults()
{
}

void rc1_start() 
{
	setup_switch(); 
	start_vlan();
}
void rc1_stop()
{
	stop_vlan();
}

void rc7_start()
{
#ifdef CONFIG_PPTP_CLIENT
	start_pptp();
#endif
#ifdef CONFIG_IPSEC_FREESWAN
	start_ipsec();
#endif
}


void rc7_stop()
{
#ifdef CONFIG_IPSEC_FREESWAN
	stop_ipsec();
#endif
#ifdef CONFIG_PPTP_CLIENT
	stop_pptp();
#endif
}



