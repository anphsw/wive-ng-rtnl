#ifndef COMPAT_SKBUFF_H
#define COMPAT_SKBUFF_H 1

struct tcphdr;
struct udphdr;

#define skb_ifindex(skb) (((skb)->input_dev != NULL) ? (skb)->input_dev->ifindex : 0)
#define skb_nfmark(skb) (((struct sk_buff *)(skb))->mark)

#ifdef CONFIG_NETWORK_SECMARK
#	define skb_secmark(skb) ((skb)->secmark)
#else
#	define skb_secmark(skb) 0
#endif

#define ipv6_hdr(skb) ((skb)->nh.ipv6h)
#define skb_network_header(skb) ((skb)->nh.raw)
#define skb_transport_header(skb) ((skb)->h.raw)
#define skb_network_header_len(skb) (skb->h.raw - skb->nh.raw)
#endif /* COMPAT_SKBUFF_H */
