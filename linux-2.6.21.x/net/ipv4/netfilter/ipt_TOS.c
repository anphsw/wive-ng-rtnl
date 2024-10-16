/* This is a module which is used for setting the TOS field of a packet. */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/checksum.h>

#include <net/netfilter/nf_conntrack.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ipt_TOS.h>

#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE)
#include "../../nat/hw_nat/ra_nat.h"
#include "../../nat/hw_nat/frame_engine.h"
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables TOS mangling module");

static unsigned int
target(struct sk_buff **pskb,
       const struct net_device *in,
       const struct net_device *out,
       unsigned int hooknum,
       const struct xt_target *target,
       const void *targinfo)
{
	const struct ipt_tos_target_info *tosinfo = targinfo;
	struct iphdr *iph = (*pskb)->nh.iph;

	if ((iph->tos & IPTOS_TOS_MASK) != tosinfo->tos) {
		__u8 oldtos;
		if (!skb_make_writable(*pskb, sizeof(struct iphdr)))
			return NF_DROP;
		iph = (*pskb)->nh.iph;
		oldtos = iph->tos;
		iph->tos = (iph->tos & IPTOS_PREC_MASK) | tosinfo->tos;
		nf_csum_replace2(&iph->check, htons(oldtos), htons(iph->tos));
	}

#ifdef CONFIG_BCM_NAT
	if(nf_conntrack_fastnat) {
	    enum ip_conntrack_info ctinfo;
	    struct nf_conn *ct = nf_ct_get(*pskb, &ctinfo);
	    ct->fastnat |= NF_FAST_NAT_DENY;
	}
#endif
#if  defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE)
	FOE_ALG_MARK(*pskb);
#endif
	return XT_CONTINUE;
}

static int
checkentry(const char *tablename,
	   const void *e_void,
	   const struct xt_target *target,
	   void *targinfo,
	   unsigned int hook_mask)
{
	const u_int8_t tos = ((struct ipt_tos_target_info *)targinfo)->tos;

	if (tos != IPTOS_LOWDELAY
	    && tos != IPTOS_THROUGHPUT
	    && tos != IPTOS_RELIABILITY
	    && tos != IPTOS_MINCOST
	    && tos != IPTOS_NORMALSVC) {
		printk(KERN_WARNING "TOS: bad tos value %#x\n", tos);
		return 0;
	}
	return 1;
}

static struct xt_target ipt_tos_reg __read_mostly = {
	.name		= "TOS",
	.family		= AF_INET,
	.target		= target,
	.targetsize	= sizeof(struct ipt_tos_target_info),
	.table		= "mangle",
	.checkentry	= checkentry,
	.me		= THIS_MODULE,
};

static int __init ipt_tos_init(void)
{
	return xt_register_target(&ipt_tos_reg);
}

static void __exit ipt_tos_fini(void)
{
	xt_unregister_target(&ipt_tos_reg);
}

module_init(ipt_tos_init);
module_exit(ipt_tos_fini);
