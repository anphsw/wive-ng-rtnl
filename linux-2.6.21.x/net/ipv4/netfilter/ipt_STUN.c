
#include <linux/config.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/netfilter.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/checksum.h>
#include <net/route.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_STUN.h>

/* for DMZ | VirtualServer */
/*FIXME: we just dun care about the differ interface when STUN apply to multi-rules. */
STUN	tcp_nat_type=__PORT_RESTRICTED_CONE, 
		udp_nat_type=__PORT_RESTRICTED_CONE;

/* Called when user tries to insert an entry of this type: 
 * hook_mask is a bitmask of hooks from which it can be called.
 * Return true or false.
 */
static int checkentry(const char *tablename, 
					const void *e, 
					const struct xt_target *target, 
					void *targinfo, 
					unsigned int targinfosize, 
					unsigned int hook_mask)
{
	/*const struct ip_nat_multi_range_compat	*mr=targinfo;*/
	
	/* Checking */
	if(strcmp(tablename, "nat")!=0)
		return	0;
	/*if (mr->range[0].flags&IP_NAT_RANGE_MAP_IPS) {
		printk("STUN_check: bad MAP_IPS.\n");
		return 0;
	}
	if (mr->rangesize!=1) {
		printk("STUN_check: bad rangesize %u.\n", mr->rangesize);
		return 0;
	}*/
	return	1;
}

/* Returns verdict */
static unsigned int target(struct sk_buff **pskb, 
						const struct net_device *in, 
						const struct net_device *out, 
						unsigned int hooknum, 
						const struct xt_target *target, 
						const void *targinfo, 
						void *userinfo)
{
	STUN		stun_state=__NONE;		/* Recording the STUN type */
	u_int32_t	newsrc;
	//struct rt_key	key;
	struct rtable	*rt;
	struct ip_conntrack		*ct;
	enum ip_conntrack_info	ctinfo;
	struct ip_nat_range		newrange;
	const struct ip_nat_multi_range_compat	*mr;
	
	IP_NF_ASSERT(hooknum==NF_IP_POST_ROUTING);
		
	/* FIXME: For the moment, don't do local packets, breaks testsuite for 2.3.49 --RR */
	/*if((*pskb)->sk) {
		return NF_ACCEPT;
	}*/
	ct=ip_conntrack_get(*pskb, &ctinfo);
	IP_NF_ASSERT(ct&&(ctinfo==IP_CT_NEW||ctinfo==IP_CT_RELATED
		||ctinfo==IP_CT_RELATED+IP_CT_IS_REPLY));
	
	/* Source address is 0.0.0.0 - locally generated packet that is
	 * probably not supposed to be masqueraded.
	 */
	if (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip == 0)
		return NF_ACCEPT;
	mr=targinfo;
	
	/* Setting STUN type */
	if(mr->range[0].flags&IP_NAT_FULL_CONE) {
		stun_state=__FULL_CONE;
	} else if(mr->range[0].flags&IP_NAT_RESTRICTED_CONE) {
		stun_state=__RESTRICTED_CONE;
	} else if(mr->range[0].flags&IP_NAT_PORT_RESTRICTED_CONE) {
		stun_state=__PORT_RESTRICTED_CONE;
	/*} else if(mr->range[0].flags&IP_NAT_SYMMETRIC) {
		stun_state=__SYMMETRIC_NAT;*/
	} else {
		stun_state=__ERROR;
		printk(KERN_ERR "Error status happens in STUN!\n");
	}
	if(ct->tuplehash[0].tuple.dst.protonum==IPPROTO_TCP) {
		if(stun_state>tcp_nat_type)
			tcp_nat_type=stun_state;
	} else if(ct->tuplehash[0].tuple.dst.protonum==IPPROTO_UDP) {
		if(stun_state>udp_nat_type)
			udp_nat_type=stun_state;
	}
	/*key.dst=(*pskb)->nh.iph->daddr;
	key.src=0;
	key.oif=0;
	key.tos=RT_TOS((*pskb)->nh.iph->tos)|RTO_CONN;*/
	rt=(struct rtable *)(*pskb)->dst;
	newsrc=inet_select_addr(out, rt->rt_gateway, RT_SCOPE_UNIVERSE);
	if (!newsrc) {
		printk("MASQUERADE: %s ate my IP address\n", out->name);
		return NF_DROP;
	}
	/*if(ip_route_output_key(&rt, &key)!=0) {
		if (net_ratelimit())
			printk("STUN: No route: Rusty's brain broke!\n");
		return NF_DROP;
	}
	if (rt->u.dst.dev!=out) {
		if (net_ratelimit())
			printk("STUN: Route sent us somewhere else.\n");
		return NF_DROP;
	}	
	newsrc=rt->rt_src;
	ip_rt_put(rt);*/
	
	/* No CONFIG_IP_NF_TARGET_MASQUERADE_MODULE, comment this! */
	/*WRITE_LOCK(&stun_rwlock);
	ct->nat.masq_index=out->ifindex;
	WRITE_UNLOCK(&stun_rwlock);
	newrange=(struct ip_nat_multi_range){1, {{mr->range[0].flags|IP_NAT_RANGE_MAP_IPS
		, newsrc, newsrc, mr->range[0].min, mr->range[0].max}}};*/
	
	/* Transfer from original range. */
	newrange=(struct ip_nat_range){mr->range[0].flags|IP_NAT_RANGE_MAP_IPS
		, newsrc, newsrc, mr->range[0].min, mr->range[0].max };
	
	/* Picking out IPPROTO_TCP or IPPROTO_UDP packets */
	if(stun_helper) {
		if(ct->tuplehash[0].tuple.dst.protonum==IPPROTO_TCP||
			ct->tuplehash[0].tuple.dst.protonum==IPPROTO_UDP) {
			return	ip_nat_setup_info_for_stun(ct, &newrange, hooknum, stun_state);
		} 
	}
	return	ip_nat_setup_info(ct, &newrange, hooknum);
}

/* Called when entry of this type deleted. */
void destroy(void *targinfo, 
			unsigned int targinfosize)
{
}

static struct ipt_target	target_stun= {
	.list		=	{NULL, NULL}, 
	.name		=	"STUN",  
	.target		=	&target, 
	.checkentry	=	&checkentry, 	
	.destroy	=	NULL, 
	.hooks		=	1<<NF_IP_POST_ROUTING, 
	.targetsize	=	sizeof(struct ip_nat_multi_range_compat), 
	.table		=	"nat", 
	.me			=	THIS_MODULE
};

static int __init init(void)
{
	/*register_inetaddr_notifier()*/
	return ipt_register_target(&target_stun);
}

static void __exit fini(void)
{
	/*unregister_inetaddr_notifier();*/
	ipt_unregister_target(&target_stun);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nonoh <06020@alphanetworks.com>");
MODULE_DESCRIPTION("iptables STUN target module");
