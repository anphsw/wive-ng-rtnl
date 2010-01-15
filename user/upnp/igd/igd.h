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
 * $Id: igd.h,v 1.1.1.1 2008/07/21 09:20:38 james26_jang Exp $
 */

#ifndef _igd_h_
#define _igd_h_

typedef struct _if_stats {
    unsigned long rx_packets;	/* total packets received       */
    unsigned long tx_packets;	/* total packets transmitted    */
    unsigned long rx_bytes;	/* total bytes received         */
    unsigned long tx_bytes;	/* total bytes transmitted      */
    unsigned long rx_errors;	/* bad packets received         */
    unsigned long tx_errors;	/* packet transmit problems     */
    unsigned long rx_dropped;	/* no space in linux buffers    */
    unsigned long tx_dropped;	/* no space available in linux  */
    unsigned long rx_multicast;	/* multicast packets received   */
    unsigned long rx_compressed;
    unsigned long tx_compressed;
    unsigned long collisions;

    /* detailed rx_errors: */
    unsigned long rx_length_errors;
    unsigned long rx_over_errors;	/* receiver ring buff overflow  */
    unsigned long rx_crc_errors;	/* recved pkt with crc error    */
    unsigned long rx_frame_errors;	/* recv'd frame alignment error */
    unsigned long rx_fifo_errors;	/* recv'r fifo overrun          */
    unsigned long rx_missed_errors;	/* receiver missed packet     */
    /* detailed tx_errors */
    unsigned long tx_aborted_errors;
    unsigned long tx_carrier_errors;
    unsigned long tx_fifo_errors;
    unsigned long tx_heartbeat_errors;
    unsigned long tx_window_errors;

} if_stats_t;

typedef struct _WANDevicePrivateData {
    char ifname[20];
} WANDevicePrivateData, *PWANDevicePrivateData;

typedef struct _LANDevicePrivateData {
    char ifname[20];
} LANDevicePrivateData, *PLANDevicePrivateData;

typedef struct _WANCommonPrivateData {
    /* link status */
    osl_link_t if_up;

    /* interface speed (bits/sec) */
    unsigned long tx_bitrate;
    unsigned long rx_bitrate;

    /* network statistics, collected through osl_ifstats */
    if_stats_t stats;

    /* event handle for periodic update event,
       created in WANCommonInterfaceConfig_Init(). */
    timer_t eventhandle;

} WANCommonPrivateData, *PWANCommonPrivateData;

extern void osl_igd_disable(char *ifname);
extern void osl_igd_enable(char *ifname);
extern char *igd_pri_wan_var(char *prefix, int len, char *var);

#define SOAP_CONNECTIONNOTCONFIGURED	706 
#define SOAP_DISCONNECTINPROGRESS	707 
#define SOAP_INVALIDCONNECTIONTYPE	710 
#define SOAP_CONNECTIONALREADYTERMNATED	711 
#define SOAP_SPECIFIEDARRAYINDEXINVALID	713 
#define SOAP_NOSUCHENTRYINARRAY		714 
#define SOAP_CONFLICTINMAPPINGENTRY	718 
#define SOAP_ONLYPERMANENTLEASESSUPPORTED	725

#if defined(linux)

/* Allow some time for the page to reload before killing ourselves */
static int
kill_after(pid_t pid, int sig, unsigned int after)
{
	if (fork() == 0) {
		sleep(after);
		return kill(pid, sig);
	}
	return 0;
}
#define sys_restart() kill_after(1, SIGHUP, 3)
#define sys_reboot() kill_after(1, SIGTERM, 3)

#endif /* linux */

#endif /* _igd_h_ */
