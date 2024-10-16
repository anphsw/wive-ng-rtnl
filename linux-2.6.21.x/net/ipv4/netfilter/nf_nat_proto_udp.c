/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2006 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/udp.h>

#include <linux/netfilter.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_core.h>
#include <net/netfilter/nf_nat_rule.h>
#include <net/netfilter/nf_nat_protocol.h>

static u_int16_t udp_port_rover;

static int
udp_unique_tuple(struct nf_conntrack_tuple *tuple,
		 const struct nf_nat_range *range,
		 enum nf_nat_manip_type maniptype,
		 const struct nf_conn *ct)
{
	return nf_nat_proto_unique_tuple(tuple, range, maniptype, ct,
					 &udp_port_rover);
}

static int
udp_manip_pkt(struct sk_buff **pskb,
	      unsigned int iphdroff,
	      const struct nf_conntrack_tuple *tuple,
	      enum nf_nat_manip_type maniptype)
{
	struct iphdr *iph = (struct iphdr *)((*pskb)->data + iphdroff);
	struct udphdr *hdr;
	unsigned int hdroff = iphdroff + iph->ihl*4;
	__be32 oldip, newip;
	__be16 *portptr, newport;

	if (!skb_make_writable(*pskb, hdroff + sizeof(*hdr)))
		return 0;

	iph = (struct iphdr *)((*pskb)->data + iphdroff);
	hdr = (struct udphdr *)((*pskb)->data + hdroff);

	if (maniptype == IP_NAT_MANIP_SRC) {
		/* Get rid of src ip and src pt */
		oldip = iph->saddr;
		newip = tuple->src.u3.ip;
		newport = tuple->src.u.udp.port;
		portptr = &hdr->source;
	} else {
		/* Get rid of dst ip and dst pt */
		oldip = iph->daddr;
		newip = tuple->dst.u3.ip;
		newport = tuple->dst.u.udp.port;
		portptr = &hdr->dest;
	}
	if (hdr->check || (*pskb)->ip_summed == CHECKSUM_PARTIAL) {
		nf_proto_csum_replace4(&hdr->check, *pskb, oldip, newip, 1);
		nf_proto_csum_replace2(&hdr->check, *pskb, *portptr, newport,
				       0);
		if (!hdr->check)
			hdr->check = CSUM_MANGLED_0;
	}
	*portptr = newport;
	return 1;
}

const struct nf_nat_protocol nf_nat_protocol_udp = {
	.name			= "UDP",
	.protonum		= IPPROTO_UDP,
	.me			= THIS_MODULE,
	.manip_pkt		= udp_manip_pkt,
	.in_range		= nf_nat_proto_in_range,
	.unique_tuple		= udp_unique_tuple,
#if defined(CONFIG_NF_CT_NETLINK) || defined(CONFIG_NF_CT_NETLINK_MODULE)
	.range_to_nfattr	= nf_nat_port_range_to_nfattr,
	.nfattr_to_range	= nf_nat_port_nfattr_to_range,
#endif
};
