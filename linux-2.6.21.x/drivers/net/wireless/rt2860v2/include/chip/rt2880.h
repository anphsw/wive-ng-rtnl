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
	rt305x.h
 
    Abstract:
 
    Revision History:
    Who          When          What
    ---------    ----------    ----------------------------------------------
 */

#ifndef __RT2880_H__
#define __RT2880_H__

#include "chip/mac_pci.h"
#include <asm/rt2880/rt_mmap.h>

#ifndef RTMP_RBUS_SUPPORT
#error "For RT2880, you should define the compile flag -DRTMP_RBUS_SUPPORT"
#endif

#ifndef RTMP_MAC_PCI
#error "For RT2880, you should define the compile flag -DRTMP_MAC_PCI"
#endif

#define RT2860_CSR_ADDR_PCI	0xC0000000 // RT2880 PCI
#define RTMP_MAC_CSR_ADDR 	RALINK_11N_MAC_BASE
#define RTMP_FLASH_BASE_ADDR	0xbfc00000

#ifdef DFS_SUPPORT
#define DFS_1_SUPPORT
#endif

#endif //__RT2880_H__ //
