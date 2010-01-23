/* Kernel module to match one of a list of TCP/UDP ports: ports are in
   the same place so we can treat them as equal. */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/udp.h>
#include <linux/skbuff.h>

#include <linux/netfilter_ipv4/ipt_mport.h>
#include <linux/netfilter_ipv4/ip_tables.h>

MODULE_LICENSE("GPL");

#if 1
#define duprintf(format, args...) printk(format , ## args)
#else
#define duprintf(format, args...)
#endif

/* Returns 1 if the port is matched by the test, 0 otherwise. */
static inline int
ports_match(const struct ipt_mport *minfo, u_int16_t src, u_int16_t dst)
{
	unsigned int i;
        unsigned int m;
        u_int16_t pflags = minfo->pflags;
	for (i=0, m=1; i<IPT_MULTI_PORTS; i++, m<<=1) {
                u_int16_t s, e;

                if (pflags & m
                    && minfo->ports[i] == 65535)
                        return 0;

                s = minfo->ports[i];

                if (pflags & m) {
                        e = minfo->ports[++i];
                        m <<= 1;
                } else
                        e = s;

                if (minfo->flags & IPT_MPORT_SOURCE
                    && src >= s && src <= e)
                        return 1;

		if (minfo->flags & IPT_MPORT_DESTINATION
		    && dst >= s && dst <= e)
			return 1;
	}

	return 0;
}

static int
match(const struct sk_buff *skb,
      const struct net_device *in,
      const struct net_device *out,
	  const struct xt_match *match,
      const void *matchinfo,
      int offset, unsigned int protoff, int *hotdrop)
{
	u16 ports[2];

	/* FIXME: This is due to the transation error of do_match()
	 *        , but it doesn't occur at 2.4.x kernel.
	 */
	const struct ipt_mport *minfo = matchinfo;
	/*minfo=offset;
	offset=hotdrop;

	if (offset)
		return 0;*/

	/* Must be big enough to read ports (both UDP and TCP have
           them at the start). */
	if (skb_copy_bits(skb, skb->nh.iph->ihl*4, ports, sizeof(ports)) < 0) {
		/* We've been asked to examine this packet, and we
		   can't.  Hence, no choice but to drop. */
			duprintf("ipt_mport:"
				 " Dropping evil offset=0 tinygram.\n");
			*hotdrop = 1;
			return 0;
	}

	return ports_match(minfo, ntohs(ports[0]), ntohs(ports[1]));
}

/* Called when user tries to insert an entry of this type. */
static int
checkentry(const char *tablename,
	   const void *ip_void,
	   const struct xt_match *match,
	   void *matchinfo,
	   unsigned int hook_mask)
{
	/* FIXME: This is due to the transation error of check_match()
	 *        , but it doesn't occur at 2.4.x kernel.
	 */
	/*matchsize=hook_mask;   
	
	if (matchsize != IPT_ALIGN(sizeof(struct ipt_mport)))
		return 0;*/
	const struct ipt_ip *ip = ip_void;

	/* Must specify proto == TCP/UDP, no unknown flags or bad count */
	return ((ip->proto == IPPROTO_TCP || ip->proto == IPPROTO_UDP)
		&& !(ip->invflags & IPT_INV_PROTO));
}

static struct ipt_match mport_match = { 
	.name		= "mport",
	.family     = AF_INET,
	.match		= &match,
	.checkentry	= &checkentry,
	.me			= THIS_MODULE,
	.matchsize	= sizeof(struct ipt_mport)/* Fix the checking-error at xt_check_match() */
};

static int __init init(void)
{
	return ipt_register_match(&mport_match);
}

static void __exit fini(void)
{
	ipt_unregister_match(&mport_match);
}

module_init(init);
module_exit(fini);
