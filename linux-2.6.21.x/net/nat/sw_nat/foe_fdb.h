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
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2006, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attempt
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    foe_fdb.h

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2006-10-06      Initial version
*/

#ifndef _FOE_FDB_WANTED
#define _FOE_FDB_WANTED


/*
 * DEFINITIONS AND MACROS
 */
#define FOE_ENTRY_LIFE_TIME	5
#define FOE_HASH_SIZE		(1<<11) //2048 
#define FOE_HASH_WAY_ORDER	3
#define FOE_HASH_WAY_SIZE	(1<<FOE_HASH_WAY_ORDER)
#define FOE_HASH_MASK		(FOE_HASH_SIZE-1)
#define FOE_THRESHOLD		1000

#define IN
#define OUT
#define INOUT

//#define NAT_DEBUG

#ifdef NAT_DEBUG
#define NAT_PRINT(fmt, args...) printk(KERN_INFO "%d: " fmt, __LINE__, ## args)
#else
#define NAT_PRINT(fmt, args...) { }
#endif


/*
 * TYPEDEFS AND STRUCTURES
 */
struct foe_tbl {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	unsigned short state:2, dst_port:2, in_pppoe:1, out_pppoe:1, learnt:1,
		       keepalive:1, is_tcp:1, ttl_ebl:1, qos_ebl:1, qos:3, in_vlan:1,
		       out_vlan:1;
#elif defined (__BIG_ENDIAN_BITFIELD)
	unsigned short out_vlan:1, in_vlan:1, qos:3, qos_ebl:1, ttl_ebl:1,
		       is_tcp:1, keepalive:1, learnt:1, out_pppoe:1, in_pppoe:1,
		       dst_port:2, state:2;
#endif
    unsigned short counter;
    unsigned long aging_time;
    unsigned long org_sip;
    unsigned long org_dip;
    unsigned short org_sport;
    unsigned short org_dport;
    unsigned short org_vlan;
    unsigned short new_vlan;
    unsigned long new_sip;
    unsigned long new_dip;
    unsigned short new_sport;
    unsigned short new_dport;
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
    unsigned short pppoe_sid;
};

struct foe_primary_key {
    unsigned long sip;
    unsigned long dip;
    unsigned short sport;
    unsigned short dport;
    unsigned short vlan;
    unsigned char is_tcp:1;
    unsigned char is_pppoe:1;
};

enum foe_get_state {
    ENTRY_FULL,
    ENTRY_NEW,
    ENTRY_FOUND
};

enum foe_entry_state {
    INVALID,
    LEARNING,
    BINDING,
    ALG
};

enum foe_entry_dp {
    GMAC0,
    GMAC1,
    WLAN0,
    WLAN1
};

#if 0
typedef enum _bool {
    false,
    true
} bool;
#endif


/*
 * EXPORT FUNCTION
 */

/**
 * @brief the foe entry has expired or not?
 *
 * @param  entry    pointer to foe entry
 * @retval 0        not expired
 * @retval 1        expired
 */
static __inline__ int foe_entry_has_expired(struct foe_tbl *entry)
{

    if (time_after(jiffies, entry->aging_time + HZ * FOE_ENTRY_LIFE_TIME)) {
        return 1;
    }

    return 0;
}

static __inline__ void foe_entry_cleanup(struct foe_tbl *entry)
{
    memset(entry, 0, sizeof(struct foe_tbl));
}


void foe_fdb_cleanup(void);
struct foe_tbl *foe_num2entry(unsigned short num);
unsigned short foe_value2num(unsigned short hash_index,
                             unsigned short way);
enum foe_get_state foe_get_entry(IN struct foe_primary_key *key,
                                 OUT unsigned long *entry_num);

#endif
