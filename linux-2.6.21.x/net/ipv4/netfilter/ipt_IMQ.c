/*
 * This target marks packets to be enqueued to an imq device
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_IMQ.h>
#include <net/netfilter/nf_conntrack.h>
#include <linux/imq.h>

#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE)
#include "../../nat/hw_nat/ra_nat.h"
#include "../../nat/hw_nat/frame_engine.h"
#endif

static unsigned int imq_target(struct sk_buff **pskb,
			       const struct net_device *in,
			       const struct net_device *out,
			       unsigned int hooknum,
			       const struct xt_target *target,
			       const void *targinfo)
{
	struct ipt_imq_info *mr = (struct ipt_imq_info*)targinfo;

	(*pskb)->imq_flags = mr->todev | IMQ_F_ENQUEUE;

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
	return IPT_CONTINUE;
}

static int imq_checkentry(const char *tablename,
			  const void *e,
			  const struct xt_target *target,
			  void *targinfo,
			  unsigned int hook_mask)
{
	struct ipt_imq_info *mr;

	mr = (struct ipt_imq_info*)targinfo;

	if (mr->todev > IMQ_MAX_DEVS) {
		printk(KERN_WARNING
		       "IMQ: invalid device specified, highest is %u\n",
		       IMQ_MAX_DEVS);
		return 0;
	}

	return 1;
}

static struct xt_target ipt_imq_reg = {
	.name		= "IMQ",
	.target		= imq_target,
	.targetsize	= sizeof(struct ipt_imq_info),
	.checkentry	= imq_checkentry,
	.me		= THIS_MODULE,
	.table		= "mangle",
	.family		= AF_INET
};

static int __init init(void)
{
	if (xt_register_target(&ipt_imq_reg))
		return -EINVAL;

	return 0;
}

static void __exit fini(void)
{
	xt_unregister_target(&ipt_imq_reg);
}

module_init(init);
module_exit(fini);

MODULE_AUTHOR("http://www.linuximq.net");
MODULE_DESCRIPTION("Pseudo-driver for the intermediate queue device. See http://www.linuximq.net/ for more information.");
MODULE_LICENSE("GPL");
