/*
 *	Forwarding decision
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	$Id: br_forward.c,v 1.1.1.1 2007-05-25 06:50:00 bruce Exp $
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

#ifdef CONFIG_BRIDGE_IGMPP_PROCFS
#include <linux/in.h>
unsigned char bcast_mac_addr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
#endif

/* Don't forward packets to originating port or forwarding diasabled */
static inline int should_deliver(struct net_bridge_port *p, 
				 const struct sk_buff *skb)
{
 	if ((skb->dev == p->dev) || (p->state != BR_STATE_FORWARDING))
 		return 0;

	return 1;
}

static inline unsigned packet_length(const struct sk_buff *skb)
{
	return skb->len - (skb->protocol == htons(ETH_P_8021Q) ? VLAN_HLEN : 0);
}

int br_dev_queue_push_xmit(struct sk_buff *skb)
{
	/* drop mtu oversized packets except gso */
	if (packet_length(skb) > skb->dev->mtu && !skb_is_gso(skb))
		kfree_skb(skb);
	else {
		/* ip_refrag calls ip_fragment, doesn't copy the MAC header. */
		if (nf_bridge_maybe_copy_header(skb))
			kfree_skb(skb);
		else {
			skb_push(skb, ETH_HLEN);

			dev_queue_xmit(skb);
		}
	}

	return 0;
}

int br_forward_finish(struct sk_buff *skb)
{
	return NF_HOOK(PF_BRIDGE, NF_BR_POST_ROUTING, skb, NULL, skb->dev,
		       br_dev_queue_push_xmit);

}

static void __br_deliver(const struct net_bridge_port *to, struct sk_buff *skb)
{
	skb->dev = to->dev;
	NF_HOOK(PF_BRIDGE, NF_BR_LOCAL_OUT, skb, NULL, skb->dev,
			br_forward_finish);
}

static void __br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
{
	struct net_device *indev;

	indev = skb->dev;
	skb->dev = to->dev;
	skb->ip_summed = CHECKSUM_NONE;

	NF_HOOK(PF_BRIDGE, NF_BR_FORWARD, skb, indev, skb->dev,
			br_forward_finish);
}

/* called with rcu_read_lock */
void br_deliver(struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_deliver(to, skb);
		return;
	}

	kfree_skb(skb);
}

/* called with rcu_read_lock */
void br_forward(struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_forward(to, skb);
		return;
	}

	kfree_skb(skb);
}
#ifdef CONFIG_BRIDGE_IGMPP_PROCFS
static void copy_mac(unsigned char* to, unsigned char * from)
{
	int i;
	for(i=0; i<6; i++)
		*(to+i)=*(from+i);
	return;
}
#endif

/* called under bridge lock */
static void br_flood(struct net_bridge *br, struct sk_buff *skb,
	void (*__packet_hook)(const struct net_bridge_port *p,
			      struct sk_buff *skb))
{
	struct net_bridge_port *p;
	struct net_bridge_port *prev;
#ifdef CONFIG_BRIDGE_IGMPP_PROCFS
	struct ethhdr *dest;
	int i;

	if(atomic_read(&br->br_igmpp_table_enable) == 1){
		list_for_each_entry_rcu(p, &br->port_list, list) {
			struct sk_buff *skb2;
			if ( atomic_read(&p->wireless_interface) == 1){ /* wireless interface */
				if (    !memcmp(eth_hdr(skb)->h_dest, bcast_mac_addr, 6) ||	/* always flooding broadcast packets */
					skb->nh.iph->protocol == IPPROTO_IGMP   )			/* always flooding IGMP packets */
				{
					if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
						br->statistics.tx_dropped++;
						kfree_skb(skb);
					return;
					}
					if (should_deliver(p,skb2))
						__packet_hook(p, skb2);
					else
						kfree_skb(skb2);
				} else {/* neither broadcast nor IGMP packet, does group address stored in table ? */
					int groupIdx;
					groupIdx = search_group_IP( &p->port_igmpp_table, skb->nh.iph->daddr);
					if (groupIdx >=0){
						/* skb_copy for each host*/
						for(i=0; i<HOSTLIST_NUMBER; i++){	
							if (p->port_igmpp_table.group_list[groupIdx].host_list[i].used ==1){
								if ((skb2 = skb_copy(skb, GFP_ATOMIC)) == NULL) {
									br->statistics.tx_dropped++;
									kfree_skb(skb);
									return;
								}
								dest = eth_hdr(skb2);
								copy_mac( dest->h_dest, p->port_igmpp_table.group_list[groupIdx].host_list[i].mac_addr);
								if (should_deliver(p, skb2))
									__packet_hook(p, skb2);
								else
									kfree_skb(skb2);
							}// if used - END
						}// for loop - END
					}else { /* skb's destination IP address does't match in port_igmpp_table, do nothing (packet will be droped) */
					}// groupIdx >=0 - END
				} //broadcast and IGMP check - END
			}else{ 
				/* it's wired interface */
				/* skb_clone..... */
				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					br->statistics.tx_dropped++;
					kfree_skb(skb);
					return;
				}
				if (should_deliver(p,skb2))
					__packet_hook(p, skb2);
				else
					kfree_skb(skb2);

			}// interface ==1 - END
		} //list_for_each_entry_rcu() - END

		kfree_skb(skb);

	}else{
		prev = NULL;

		list_for_each_entry_rcu(p, &br->port_list, list) {
			if (should_deliver(p, skb)) {
				if (prev != NULL) {
					struct sk_buff *skb2;

					if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
						br->statistics.tx_dropped++;
						kfree_skb(skb);
						return;
					}

					__packet_hook(prev, skb2);
				}

				prev = p;
			}
		}

		if (prev != NULL) {
			__packet_hook(prev, skb);
			return;
		}

		kfree_skb(skb);
	}
#else
	prev = NULL;

	list_for_each_entry_rcu(p, &br->port_list, list) {
		if (should_deliver(p, skb)) {
			if (prev != NULL) {
				struct sk_buff *skb2;

				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					br->statistics.tx_dropped++;
					kfree_skb(skb);
					return;
				}

				__packet_hook(prev, skb2);
			}

			prev = p;
		}
	}

	if (prev != NULL) {
		__packet_hook(prev, skb);
		return;
	}

	kfree_skb(skb);
#endif
}


/* called with rcu_read_lock */
void br_flood_deliver(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, __br_deliver);
}

/* called under bridge lock */
void br_flood_forward(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, __br_forward);
}
