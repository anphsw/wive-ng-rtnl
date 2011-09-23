/*
 *	Handle incoming frames
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	$Id: br_input.c,v 1.2 2010-11-24 03:40:46 yy Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

#if defined(CONFIG_BRIDGE_IGMPP_PROCFS) || defined(CONFIG_BRIDGE_IGMP_REPORT_NO_FLOODING)
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/igmp.h>
#endif

/* Bridge group multicast address 802.1d (pg 51). */
const u8 br_group_address[ETH_ALEN] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };

static void br_pass_frame_up(struct net_bridge *br, struct sk_buff *skb)
{
	struct net_device *indev;

	br->statistics.rx_packets++;
	br->statistics.rx_bytes += skb->len;

	indev = skb->dev;
	skb->dev = br->dev;

	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, indev, NULL,
		netif_receive_skb);
}

#ifdef CONFIG_BRIDGE_IGMPP_PROCFS
/* snoop_MAC() => If IP address that existed in br_mac_table ,replace it,
 * else create a new list entry and add it to list.
 * called under bridge lock */
static void snoop_MAC(struct net_bridge *br ,struct sk_buff *skb2)
{
	uint32_t ip32 =  (uint32_t) skb2->nh.iph->saddr;

	struct br_mac_table_t *tlist;
	struct ethhdr *src;
	int find = 0, i = 0;

	list_for_each_entry(tlist,&(br->br_mac_table.list), list){
		if ( tlist->ip_addr == ip32){
			find =1;
			src = eth_hdr(skb2);
			for (i =0; i<6; i++)
				tlist->mac_addr[i] = src->h_source[i];
			break;
		}
	}
	if (find == 0 ){
		struct br_mac_table_t * new_entry;
		new_entry = (struct br_mac_table_t *)kmalloc(sizeof(struct br_mac_table_t), GFP_ATOMIC);
		if (new_entry != NULL){
			int i;
			struct ethhdr * src = eth_hdr(skb2);
			for (i =0; i<6; i++)
				new_entry->mac_addr[i] = src->h_source[i];
			new_entry->ip_addr = ip32;
			list_add(&(new_entry->list), &(br->br_mac_table.list));
		}else{
			#ifdef CONFIG_BRIDGE_IGMPP_PROCFS_DEBUG
			printk(KERN_INFO "[BR_MAC_PROC]-> alloc new br_mac_table_t fail !!\n");
			#endif
		}
	}
}
#endif


/* note: already called with rcu_read_lock (preempt_disabled) */
int br_handle_frame_finish(struct sk_buff *skb)
{
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	struct net_bridge_port *p = rcu_dereference(skb->dev->br_port);
	struct net_bridge *br;
	struct net_bridge_fdb_entry *dst;
        struct sk_buff *skb2;
#ifdef CONFIG_BRIDGE_IGMP_REPORT_NO_FLOODING
	struct ethhdr *eth;
	struct iphdr *ih_br;
	struct igmphdr *igmph;
#endif
#ifdef CONFIG_BRIDGE_IGMPP_PROCFS
	struct igmphdr *ih;
#endif

	if (!p || p->state == BR_STATE_DISABLED)
		goto drop;

	/* insert into forwarding database after filtering to avoid spoofing */
	br = p->br;
	br_fdb_update(br, p, eth_hdr(skb)->h_source);

#ifdef CONFIG_BRIDGE_EAP
	if ((p->state == BR_STATE_LEARNING) && skb->protocol != htons(ETH_P_PAE))
#else
	if (p->state == BR_STATE_LEARNING)
#endif
		goto drop;

	/* The packet skb2 goes to the local host (NULL to skip). */
	skb2 = NULL;

	if (br->dev->flags & IFF_PROMISC)
		skb2 = skb;

	dst = NULL;

#ifdef CONFIG_BRIDGE_EAP
	if (skb->protocol == htons(ETH_P_PAE)) {
		skb2 = skb;
		/* Do not forward 802.1x/EAP frames */
		skb = NULL;
	} else 
#endif
	if (is_multicast_ether_addr(dest)) {
#ifdef CONFIG_BRIDGE_IGMP_REPORT_NO_FLOODING
		if(dest[0] != 0x01 || dest[1] != 0x00 || dest[2] != 0x5e || (dest[3] > 0x7f))
			goto out_igmp;

		eth = (struct ethhdr *)eth_hdr(skb);

		if(eth->h_proto == htons(ETH_P_IP)){
			if(skb->len < (sizeof(struct iphdr) + sizeof(struct igmphdr)))
				goto out_igmp;

			ih_br = (struct iphdr *)skb->h.raw;
			if(ih_br->protocol != IPPROTO_IGMP)
				goto out_igmp;

			igmph = (struct igmphdr *)((unsigned char *)skb->h.raw + (ih_br->ihl<<2)  );
			if(	igmph->type == IGMP_HOST_MEMBERSHIP_REPORT ||
				igmph->type == IGMPV2_HOST_MEMBERSHIP_REPORT ||
				igmph->type == IGMPV3_HOST_MEMBERSHIP_REPORT){
				if (skb) {
				    br_pass_frame_up(br, skb);
				    goto out;
				}
			}
		}
out_igmp:
#endif
#ifdef CONFIG_BRIDGE_IGMPP_PROCFS
		spin_lock_bh(&br->lock); // bridge lock

		if (atomic_read(&br->br_mac_table_enable) == 1 )
			if(skb->nh.iph->protocol == IPPROTO_IGMP){ // IGMP protocol number: 0x02
				struct sk_buff *skb2;
				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) != NULL) {
					skb_pull(skb2, skb2->nh.iph->ihl<<2);
					ih = (struct igmphdr *) skb2->data;
					if (ih->type == IGMP_HOST_MEMBERSHIP_REPORT ||		// IGMPv1 REPORT
						ih->type == IGMPV2_HOST_MEMBERSHIP_REPORT ||	// IGMPv2 REPORT
						ih->type == IGMPV3_HOST_MEMBERSHIP_REPORT	)	// IGMPv3 REPORT
					{
						snoop_MAC(br, skb2);
					}
					kfree_skb(skb2);
				}else{
					#ifdef CONFIG_BRIDGE_IGMPP_PROCFS_DEBUG
					printk(KERN_INFO "[BR_MAC_PROC]-> alloc new sk_buff fail !!\n");
					#endif
				}
			}

		spin_unlock_bh(&br->lock); // bridge unlock
#endif
		br->statistics.multicast++;
		skb2 = skb;
	} else if ((dst = __br_fdb_get(br, dest)) && dst->is_local) {
		skb2 = skb;
		/* Do not forward the packet since it's local. */
		skb = NULL;
	}

	if (skb2 == skb)
	    skb2 = skb_clone(skb, GFP_ATOMIC);

#ifdef CONFIG_BRIDGE_FORWARD_CTRL
	if (dst != NULL && !atomic_read(&br->br_forward)) {
		kfree_skb(skb);
		br_fdb_put(dst);
		goto out;
	}
#endif
#ifdef CONFIG_BRIDGE_PORT_FORWARD
	if (dst != NULL && !p->port_forwarding) {
		kfree_skb(skb);
		br_fdb_put(dst);
		goto out;
	}
#endif
#ifdef CONFIG_BRIDGE_PORT_FORWARD
	if (dst != NULL && !p->port_forwarding) {
            kfree_skb(skb);
            br_fdb_put(dst);
            goto out;
	}
#endif
	if (skb2)
	    br_pass_frame_up(br, skb2);

	if (skb) {
		if (dst)
			br_forward(dst->dst, skb);
		else
			 br_flood_forward(br, skb);
	}

out:
	return 0;
drop:
	kfree_skb(skb);
	goto out;
}

/* note: already called with rcu_read_lock (preempt_disabled) */
static int br_handle_local_finish(struct sk_buff *skb)
{
	struct net_bridge_port *p = rcu_dereference(skb->dev->br_port);

	if (p)
		br_fdb_update(p->br, p, eth_hdr(skb)->h_source);
	return 0;	 /* process further */
}

/* Does address match the link local multicast address.
 * 01:80:c2:00:00:0X
 */
static inline int is_link_local(const unsigned char *dest)
{
        __be16 *a = (__be16 *)dest;
        static const __be16 *b = (const __be16 *)br_group_address;
        static const __be16 m = __constant_cpu_to_be16(0xfff0);

        return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | ((a[2] ^ b[2]) & m)) == 0;
}

/*
 * Called via br_handle_frame_hook.
 * Return NULL if skb is handled
 * note: already called with rcu_read_lock (preempt_disabled)
 */
struct sk_buff *br_handle_frame(struct net_bridge_port *p, struct sk_buff *skb)
{
    const unsigned char *dest = eth_hdr(skb)->h_dest;

    if (!is_valid_ether_addr(eth_hdr(skb)->h_source))
	goto drop;

    if (unlikely(is_link_local(dest))) {
        /* Pause frames shouldn't be passed up by driver anyway */
        if (skb->protocol == htons(ETH_P_PAUSE))
                    goto drop;

	/* If STP is turned off, then forward */
	if ((!p->br->stp_enabled) && dest[5] == 0)
                    goto forward;

	skb->pkt_type = PACKET_HOST;
	return (NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_IN, skb, skb->dev,
		NULL, br_handle_local_finish) == 0) ? skb : NULL;
    }

forward:
    switch (p->state) {
    case BR_STATE_FORWARDING:

	if (br_should_route_hook) {
	    if (br_should_route_hook(&skb))
		return skb;
	    dest = eth_hdr(skb)->h_dest;
	}
	/* fall through */
    case BR_STATE_LEARNING:
	if (!compare_ether_addr(p->br->dev->dev_addr, dest))
	    skb->pkt_type = PACKET_HOST;

	NF_HOOK(PF_BRIDGE, NF_BR_PRE_ROUTING, skb, skb->dev, NULL,
	    br_handle_frame_finish);
	break;
    default:
drop:
	kfree_skb(skb);
    }
    return NULL;
}
