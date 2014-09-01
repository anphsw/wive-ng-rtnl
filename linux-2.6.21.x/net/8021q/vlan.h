#ifndef __BEN_VLAN_802_1Q_INC__
#define __BEN_VLAN_802_1Q_INC__

#include <net/arp.h>
#include <linux/if_vlan.h>

/*  Uncomment this if you want debug traces to be shown. */
/* #define VLAN_DEBUG */

#define VLAN_ERR KERN_ERR
#define VLAN_INF KERN_INFO
#define VLAN_DBG KERN_ALERT /* change these... to debug, having a hard time
			     * changing the log level at run-time..for some reason.
			     */

/*

These I use for memory debugging.  I feared a leak at one time, but
I never found it..and the problem seems to have dissappeared.  Still,
I'll bet they might prove useful again... --Ben


#define VLAN_MEM_DBG(x, y, z) printk(VLAN_DBG "%s:  "  x, __FUNCTION__, y, z);
#define VLAN_FMEM_DBG(x, y) printk(VLAN_DBG "%s:  " x, __FUNCTION__, y);
*/

/* This way they don't do anything! */
#define VLAN_MEM_DBG(x, y, z)
#define VLAN_FMEM_DBG(x, y)


extern unsigned short vlan_name_type;

#define VLAN_GRP_HASH_SHIFT	5
#define VLAN_GRP_HASH_SIZE	(1 << VLAN_GRP_HASH_SHIFT)
#define VLAN_GRP_HASH_MASK	(VLAN_GRP_HASH_SIZE - 1)

/*  Find a VLAN device by the MAC address of its Ethernet device, and
 *  it's VLAN ID.  The default configuration is to have VLAN's scope
 *  to be box-wide, so the MAC will be ignored.  The mac will only be
 *  looked at if we are configured to have a separate set of VLANs per
 *  each MAC addressable interface.  Note that this latter option does
 *  NOT follow the spec for VLANs, but may be useful for doing very
 *  large quantities of VLAN MUX/DEMUX onto FrameRelay or ATM PVCs.
 *
 *  Must be invoked with rcu_read_lock (ie preempt disabled)
 *  or with RTNL.
 */

/* Our listing of VLAN group(s) */
extern struct hlist_head vlan_group_hash[VLAN_GRP_HASH_SIZE];
#define vlan_grp_hashfn(IDX)    ((((IDX) >> VLAN_GRP_HASH_SHIFT) ^ (IDX)) & VLAN_GRP_HASH_MASK)

/* Must be invoked with RCU read lock (no preempt) */
static inline struct vlan_group *vlan_find_group(int real_dev_ifindex)
{
	struct vlan_group *grp;
	struct hlist_node *n;
	int hash = vlan_grp_hashfn(real_dev_ifindex);

	hlist_for_each_entry_rcu(grp, n, &vlan_group_hash[hash], hlist) {
		if (grp->real_dev_ifindex == real_dev_ifindex)
			return grp;
	}

	return NULL;
}


/*  Find the protocol handler.  Assumes VID < VLAN_VID_MASK.
 *
 * Must be invoked with RCU read lock (no preempt)
 */
static inline struct net_device *find_vlan_dev(struct net_device *real_dev, unsigned short VID)
{
	struct vlan_group *grp = vlan_find_group(real_dev->ifindex);

	if (grp)
		return vlan_group_get_device(grp, VID);

	return NULL;
}


/*
 *	Rebuild the Ethernet MAC header. This is called after an ARP
 *	(or in future other address resolution) has completed on this
 *	sk_buff. We now let ARP fill in the other fields.
 *
 *	This routine CANNOT use cached dst->neigh!
 *	Really, it is used only when dst->neigh is wrong.
 *
 * TODO:  This needs a checkup, I'm ignorant here. --BLG
 */
static inline int vlan_dev_rebuild_header(struct sk_buff *skb)
{
	struct net_device *dev = skb->dev;
	struct vlan_ethhdr *veth = (struct vlan_ethhdr *)(skb->data);

	switch (veth->h_vlan_encapsulated_proto) {
#ifdef CONFIG_INET
	case __constant_htons(ETH_P_IP):

		/* TODO:  Confirm this will work with VLAN headers... */
		return arp_find(veth->h_dest, skb);
#endif
	default:
		printk(VLAN_DBG
		       "%s: unable to resolve type %X addresses.\n",
		       dev->name, ntohs(veth->h_vlan_encapsulated_proto));

		memcpy(veth->h_source, dev->dev_addr, ETH_ALEN);
		break;
	};

	return 0;
}

/* found in vlan_dev.c */
int vlan_skb_recv(struct sk_buff *skb, struct net_device *dev,
		  struct packet_type *ptype, struct net_device *orig_dev);
int vlan_dev_hard_header(struct sk_buff *skb, struct net_device *dev,
			 unsigned short type, void *daddr, void *saddr,
			 unsigned len);
int vlan_dev_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);
int vlan_dev_hwaccel_hard_start_xmit(struct sk_buff *skb, struct net_device *dev);
int vlan_dev_change_mtu(struct net_device *dev, int new_mtu);
int vlan_dev_set_mac_address(struct net_device *dev, void* addr);
int vlan_dev_open(struct net_device* dev);
int vlan_dev_stop(struct net_device* dev);
int vlan_dev_ioctl(struct net_device* dev, struct ifreq *ifr, int cmd);
int vlan_dev_set_ingress_priority(char* dev_name, __u32 skb_prio, short vlan_prio);
int vlan_dev_set_egress_priority(char* dev_name, __u32 skb_prio, short vlan_prio);
int vlan_dev_set_vlan_flag(char* dev_name, __u32 flag, short flag_val);
int vlan_dev_get_realdev_name(const char* dev_name, char* result);
int vlan_dev_get_vid(const char* dev_name, unsigned short* result);
void vlan_dev_set_multicast_list(struct net_device *vlan_dev);

#endif /* !(__BEN_VLAN_802_1Q_INC__) */
