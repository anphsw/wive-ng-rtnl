#ifndef _BCM_NAT_WANTED
#define _BCM_NAT_WANTED

#include <net/ip.h>
#include <net/route.h>
#include <net/neighbour.h>

//#define DEBUG

/*
 * fastnat must full skip mcast/bcast traffic
 * allow only tcp with established/reply pkts (fix mss correction bug in syn pkts)
 * or udp proto for processing in software offload
 */
#define FASTNAT_SKIP_TYPE(skb)		((skb)->pkt_type == PACKET_BROADCAST || (skb)->pkt_type == PACKET_MULTICAST)
#define FASTNAT_SKIP_PROTO(protonum)	(protonum != IPPROTO_UDP && protonum != IPPROTO_TCP)
#define FASTNAT_ESTABLISHED(ctinfo)	(ctinfo == IP_CT_ESTABLISHED || ctinfo == IP_CT_ESTABLISHED_REPLY)

/*
 * cb fastforward flag
 * offcet do not be replaced in any code (as hw_nat, strongly check this, now find max cb offset in wifi drivers = 41)
 */
#define NF_FAST_ROUTE		42

/*
 * conntrack fastforward flags
 * please undefine NF_STOP in linux/netfilter.h if fastnat use
 */
#define	NF_FAST_NAT_DENY	1
#define NF_FAST_NAT		5
#define NF_STOP			6

extern unsigned int nf_conntrack_fastnat;
extern unsigned int nf_conntrack_fastroute;

extern int nf_ct_ipv4_gather_frags(struct sk_buff *skb, u_int32_t user);
extern int ip_rcv_finish(struct sk_buff *skb);
#endif
