#ifndef _NF_NAT_CORE_H
#define _NF_NAT_CORE_H
#include <linux/list.h>
#include <net/netfilter/nf_conntrack.h>

/* This header used to share core functionality between the standalone
   NAT module, and the compatibility layer's use of NAT for masquerading. */

extern unsigned int nf_nat_packet(struct nf_conn *ct,
				  enum ip_conntrack_info ctinfo,
				  unsigned int hooknum,
				  struct sk_buff **pskb);

extern int nf_nat_icmp_reply_translation(struct nf_conn *ct,
					 enum ip_conntrack_info ctinfo,
					 unsigned int hooknum,
					 struct sk_buff **pskb);

static inline int nf_nat_initialized(struct nf_conn *ct,
				     enum nf_nat_manip_type manip)
{
	if (manip == IP_NAT_MANIP_SRC)
		return ct->status & IPS_SRC_NAT_DONE;
	else
		return ct->status & IPS_DST_NAT_DONE;
}
#endif /* _NF_NAT_CORE_H */
