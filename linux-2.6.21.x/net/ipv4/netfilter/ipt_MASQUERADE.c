/* Masquerade.  Simple mapping which alters range to a local IP address
   (depending on route). */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2006 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/inetdevice.h>
#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <net/protocol.h>
#include <net/ip.h>
#include <net/checksum.h>
#include <net/route.h>
#include <net/netfilter/nf_nat_rule.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("Xtables: automatic-address SNAT");

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

/* FIXME: Multiple targets. --RR */
static int
masquerade_check(const char *tablename,
		 const void *e,
		 const struct xt_target *target,
		 void *targinfo,
		 unsigned int hook_mask)
{
	const struct nf_nat_multi_range_compat *mr = targinfo;

	if (mr->range[0].flags & IP_NAT_RANGE_MAP_IPS) {
		DEBUGP("masquerade_check: bad MAP_IPS.\n");
		return 0;
	}
	if (mr->rangesize != 1) {
		DEBUGP("masquerade_check: bad rangesize %u.\n", mr->rangesize);
		return 0;
	}
	return 1;
}

static unsigned int
masquerade_target(struct sk_buff **pskb,
		  const struct net_device *in,
		  const struct net_device *out,
		  unsigned int hooknum,
		  const struct xt_target *target,
		  const void *targinfo)
{
	struct nf_conn *ct;
	struct nf_conn_nat *nat;
	enum ip_conntrack_info ctinfo;
	struct nf_nat_range newrange;
	const struct nf_nat_multi_range_compat *mr;
	struct rtable *rt;
	__be32 newsrc;

	NF_CT_ASSERT(hooknum == NF_IP_POST_ROUTING);

	ct = nf_ct_get(*pskb, &ctinfo);
	nat = nfct_nat(ct);

	NF_CT_ASSERT(ct && (ctinfo == IP_CT_NEW || ctinfo == IP_CT_RELATED ||
			    ctinfo == IP_CT_RELATED_REPLY));

	/* Source address is 0.0.0.0 - locally generated packet that is
	 * probably not supposed to be masqueraded.
	 */
	if (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip == 0)
		return NF_ACCEPT;

	mr = targinfo;

	{
		struct flowi fl = { .nl_u = { .ip4_u =
					      { .daddr = (*pskb)->nh.iph->daddr,
						.tos = (RT_TOS((*pskb)->nh.iph->tos)),
						.gw = ((struct rtable *) (*pskb)->dst)->rt_gateway,
					      } },
				    .mark = (*pskb)->mark,
				    .oif = out->ifindex };
		if (ip_route_output_key(&rt, &fl) != 0) {
			/* Funky routing can do this. */
			if (net_ratelimit())
				printk("MASQUERADE:"
				       " No route: Rusty's brain broke!\n");
			return NF_DROP;
		}
	}

	newsrc = rt->rt_src;
	DEBUGP("newsrc = %u.%u.%u.%u\n", NIPQUAD(newsrc));
	ip_rt_put(rt);

	nat->masq_index = out->ifindex;

	/* Transfer from original range. */
	newrange = ((struct nf_nat_range)
		{ mr->range[0].flags | IP_NAT_RANGE_MAP_IPS,
		  newsrc, newsrc,
		  mr->range[0].min, mr->range[0].max });

	/* Hand modified range to generic setup. */
	return nf_nat_setup_info(ct, &newrange, hooknum);
}

static inline int
device_cmp(struct nf_conn *i, void *ifindex)
{
	const struct nf_conn_nat *nat = nfct_nat(i);

	if (!nat)
		return 0;
	return nat->masq_index == (int)(long)ifindex;
}

static int masq_device_event(struct notifier_block *this,
			     unsigned long event,
			     void *ptr)
{
	const struct net_device *dev = ptr;

	if (event == NETDEV_DOWN) {
		/* Device was downed.  Search entire table for
		   conntracks which were associated with that device,
		   and forget them. */
		NF_CT_ASSERT(dev->ifindex != 0);

		nf_ct_iterate_cleanup(device_cmp, (void *)(long)dev->ifindex);
	}

	return NOTIFY_DONE;
}

static int masq_inet_event(struct notifier_block *this,
			   unsigned long event,
			   void *ptr)
{
	struct net_device *dev = ((struct in_ifaddr *)ptr)->ifa_dev->dev;
        return masq_device_event(this, event, dev);
}

static struct notifier_block masq_dev_notifier = {
	.notifier_call	= masq_device_event,
};

static struct notifier_block masq_inet_notifier = {
	.notifier_call	= masq_inet_event,
};

static struct xt_target masquerade __read_mostly = {
	.name		= "MASQUERADE",
	.family		= AF_INET,
	.target		= masquerade_target,
	.targetsize	= sizeof(struct nf_nat_multi_range_compat),
	.table		= "nat",
	.hooks		= 1 << NF_IP_POST_ROUTING,
	.checkentry	= masquerade_check,
	.me		= THIS_MODULE,
};

static int __init ipt_masquerade_init(void)
{
	int ret;

	ret = xt_register_target(&masquerade);

	if (ret == 0) {
		/* Register for device down reports */
		register_netdevice_notifier(&masq_dev_notifier);
		/* Register IP address change reports */
		register_inetaddr_notifier(&masq_inet_notifier);
	}

	return ret;
}

static void __exit ipt_masquerade_fini(void)
{
	xt_unregister_target(&masquerade);
	unregister_netdevice_notifier(&masq_dev_notifier);
	unregister_inetaddr_notifier(&masq_inet_notifier);
}

module_init(ipt_masquerade_init);
module_exit(ipt_masquerade_fini);
