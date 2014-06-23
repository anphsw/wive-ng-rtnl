/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Pseudo-driver for the loopback interface.
 *
 * Version:	@(#)loopback.c	1.0.4b	08/16/93
 *
 * Authors:	Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Donald Becker, <becker@scyld.com>
 *
 *		Alan Cox	:	Fixed oddments for NET3.014
 *		Alan Cox	:	Rejig for NET3.029 snap #3
 *		Alan Cox	: 	Fixed NET3.029 bugs and sped up
 *		Larry McVoy	:	Tiny tweak to double performance
 *		Alan Cox	:	Backed out LMV's tweak - the linux mm
 *					can't take it...
 *              Michael Griffith:       Don't bother computing the checksums
 *                                      on packets received on the loopback
 *                                      interface.
 *		Alexey Kuznetsov:	Potential hang under some extreme
 *					cases removed.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/socket.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/in.h>
#include <linux/init.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <net/checksum.h>
#include <linux/if_ether.h>	/* For the statistics structure. */
#include <linux/if_arp.h>	/* For ARPHRD_ETHER */
#include <linux/ip.h>
#include <linux/percpu.h>

struct pcpu_lstats {
	unsigned long packets;
	unsigned long bytes;
	unsigned long drops;
};
static DEFINE_PER_CPU(struct pcpu_lstats, pcpu_lstats);

#define LOOPBACK_OVERHEAD (128 + MAX_HEADER + 16 + 16)

/*
 * The higher levels take care of making this non-reentrant (it's
 * called with bh's disabled).
 */
static int loopback_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct pcpu_lstats *lb_stats;
	int len;

	skb_orphan(skb);

	skb->protocol = eth_type_trans(skb, dev);
#ifndef LOOPBACK_MUST_CHECKSUM
	skb->ip_summed = CHECKSUM_UNNECESSARY;
#endif
	/* it's OK to use __get_cpu_var() because BHs are off */
	lb_stats = &__get_cpu_var(pcpu_lstats);

	len = skb->len;
	if (likely(netif_rx(skb) == NET_RX_SUCCESS)) {
		lb_stats->bytes += len;
		lb_stats->packets++;
	} else
		lb_stats->drops++;

	return 0;
}

static struct net_device_stats loopback_stats;

static struct net_device_stats *get_stats(struct net_device *dev)
{
	struct net_device_stats *stats = &loopback_stats;
	unsigned long bytes = 0;
	unsigned long packets = 0;
	unsigned long drops = 0;
	int i;

	for_each_possible_cpu(i) {
		const struct pcpu_lstats *lb_stats;

		lb_stats = &per_cpu(pcpu_lstats, i);
		bytes   += lb_stats->bytes;
		packets += lb_stats->packets;
		drops   += lb_stats->drops;
	}
	stats->rx_packets = packets;
	stats->tx_packets = packets;
	stats->rx_dropped = drops;
	stats->rx_errors  = drops;
	stats->rx_bytes   = bytes;
	stats->tx_bytes   = bytes;
	return stats;
}

static u32 always_on(struct net_device *dev)
{
	return 1;
}

static const struct ethtool_ops loopback_ethtool_ops = {
	.get_link		= always_on,
	.get_tso		= ethtool_op_get_tso,
	.set_tso		= ethtool_op_set_tso,
	.get_tx_csum		= always_on,
	.get_sg			= always_on,
	.get_rx_csum		= always_on,
};

/*
 * The loopback device is special. There is only one instance and
 * it is statically allocated. Don't do this for other devices.
 */
struct net_device loopback_dev = {
	.name	 		= "lo",
	.get_stats		= &get_stats,
	.priv			= &loopback_stats,
	.mtu			= 64 * 1024,
	.hard_start_xmit	= loopback_xmit,
	.hard_header		= eth_header,
	.hard_header_cache	= eth_header_cache,
	.header_cache_update	= eth_header_cache_update,
	.hard_header_len	= ETH_HLEN,	/* 14	*/
	.addr_len		= ETH_ALEN,	/* 6	*/
	.tx_queue_len		= 0,
	.type			= ARPHRD_LOOPBACK,	/* 0x0001*/
	.rebuild_header		= eth_rebuild_header,
	.flags			= IFF_LOOPBACK,
	.features 		= NETIF_F_SG | NETIF_F_FRAGLIST
#ifdef LOOPBACK_TSO
				  | NETIF_F_TSO
#endif
				  | NETIF_F_HW_CSUM | NETIF_F_HIGHDMA
				  | NETIF_F_LLTX
				  | NETIF_F_VLAN_CHALLENGED,
	.ethtool_ops		= &loopback_ethtool_ops,
};

/* Setup and register the loopback device. */
static int __init loopback_init(void)
{
	int err = register_netdev(&loopback_dev);

	if (err)
		panic("loopback: Failed to register netdevice: %d\n", err);

	return err;
};

module_init(loopback_init);

EXPORT_SYMBOL(loopback_dev);
