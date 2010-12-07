/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
	rtmp_rbs.h
 
    Abstract:
 	Ralink SoC Internal Bus related definitions and data dtructures
 	
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */
 
#ifndef __RTMP_RBUS_H__
#define __RTMP_RBUS_H__

#include "rt_config.h"

/*************************************************************************
  *
  *	Device hardware/ Interface related definitions.
  *
  ************************************************************************/  

#define RTMP_MAC_IRQ_NUM		4
#define CMDTHREAD_CHAN_RESCAN		0x0D730101

/*************************************************************************
  *
  *	Device Tx/Rx related definitions.
  *
  ************************************************************************/
#ifdef LINUX
#if !defined(CONFIG_RA_NAT_NONE)
extern int (*ra_sw_nat_hook_tx)(struct sk_buff *skb);
extern int (*ra_sw_nat_hook_rx)(struct sk_buff *skb);
#endif
#ifdef CARRIER_DETECTION_SUPPORT
extern void unregister_tmr_service(void);
extern void request_tmr_service(int, void *, void *);
#endif
#endif /* LINUX */

#endif /* __RTMP_RBUS_H__ */
