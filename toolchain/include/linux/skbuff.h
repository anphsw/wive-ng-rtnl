/*
 *	Definitions for the 'struct sk_buff' memory handlers.
 *
 *	Authors:
 *		Alan Cox, <gw4pts@gw4pts.ampr.org>
 *		Florian La Roche, <rzsfl@rz.uni-sb.de>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifndef _LINUX_SKBUFF_H
#define _LINUX_SKBUFF_H

#include <asm/atomic.h>
#include <asm/types.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/cache.h>
#include <linux/spinlock.h>
#include <linux/net.h>
#include <linux/textsearch.h>
#include <linux/rcupdate.h>
#include <linux/dmaengine.h>
#include <linux/hrtimer.h>
#include <net/checksum.h>

/* Don't change this without changing skb_csum_unnecessary! */
#define CHECKSUM_NONE 0
#define CHECKSUM_UNNECESSARY 1
#define CHECKSUM_COMPLETE 2
#define CHECKSUM_PARTIAL 3

#define SKB_DATA_ALIGN(X)	ALIGN(X, SMP_CACHE_BYTES)
#define SKB_WITH_OVERHEAD(X)	\
	((X) - SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))
#define SKB_MAX_ORDER(X, ORDER) \
	SKB_WITH_OVERHEAD((PAGE_SIZE << (ORDER)) - (X))
#define SKB_MAX_HEAD(X)		(SKB_MAX_ORDER((X), 0))
#define SKB_MAX_ALLOC		(SKB_MAX_ORDER(0, 2))

/* return minimum truesize of one skb containing X bytes of data */
#define SKB_TRUESIZE(X) ((X) +						\
			 SKB_DATA_ALIGN(sizeof(struct sk_buff)) +	\
			 SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

/* A. Checksumming of received packets by device.
 *
 *	NONE: device failed to checksum this packet.
 *		skb->csum is undefined.
 *
 *	UNNECESSARY: device parsed packet and wouldbe verified checksum.
 *		skb->csum is undefined.
 *	      It is bad option, but, unfortunately, many of vendors do this.
 *	      Apparently with secret goal to sell you new device, when you
 *	      will add new protocol to your host. F.e. IPv6. 8)
 *
 *	COMPLETE: the most generic way. Device supplied checksum of _all_
 *	    the packet as seen by netif_rx in skb->csum.
 *	    NOTE: Even if device supports only some protocols, but
 *	    is able to produce some skb->csum, it MUST use COMPLETE,
 *	    not UNNECESSARY.
 *
 * B. Checksumming on output.
 *
 *	NONE: skb is checksummed by protocol or csum is not required.
 *
 *	PARTIAL: device is required to csum packet as seen by hard_start_xmit
 *	from skb->h.raw to the end and to record the checksum
 *	at skb->h.raw+skb->csum.
 *
 *	Device must show its capabilities in dev->features, set
 *	at device setup time.
 *	NETIF_F_HW_CSUM	- it is clever device, it is able to checksum
 *			  everything.
 *	NETIF_F_IP_CSUM - device is dumb. It is able to csum only
 *			  TCP/UDP over IPv4. Sigh. Vendors like this
 *			  way by an unknown reason. Though, see comment above
 *			  about CHECKSUM_UNNECESSARY. 8)
 *	NETIF_F_IPV6_CSUM about as dumb as the last one but does IPv6 instead.
 *
 *	Any questions? No questions, good. 		--ANK
 */

struct net_device;

#ifdef CONFIG_NETFILTER
struct nf_conntrack {
	atomic_t use;
};

#ifdef CONFIG_BRIDGE_NETFILTER
struct nf_bridge_info {
	atomic_t		use;
	unsigned int		mask;
	struct net_device	*physindev;
	struct net_device	*physoutdev;
	unsigned long		data[32 / sizeof(unsigned long)];
};
#endif

#endif

struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;

	__u32		qlen;
	spinlock_t	lock;
};

struct sk_buff;

/* To allow 64K frame to be packed as single skb without frag_list we
 * require 64K/PAGE_SIZE pages plus 1 additional page to allow for
 * buffers which do not start on a page boundary.
 *
 * Since GRO uses frags we allocate at least 16 regardless of page
 * size.
 */
#if (65536/PAGE_SIZE + 1) < 16
#define MAX_SKB_FRAGS 16UL
#else
#define MAX_SKB_FRAGS (65536/PAGE_SIZE + 1)
#endif

typedef struct skb_frag_struct skb_frag_t;

struct skb_frag_struct {
	struct page *page;
#if (BITS_PER_LONG > 32) || (PAGE_SIZE >= 65536)
	__u32 page_offset;
	__u32 size;
#else
	__u16 page_offset;
	__u16 size;
#endif
};

/* This data is invariant across clones and lives at
 * the end of the header data, ie. at skb->end.
 */
struct skb_shared_info {
	unsigned short	nr_frags;
	unsigned short	gso_size;
	/* Warning: this field is not always filled in (UFO)! */
	unsigned short	gso_segs;
	unsigned short  gso_type;
	struct sk_buff	*frag_list;
	__be32          ip6_frag_id;

	/*
	 * Warning : all fields before dataref are cleared in __alloc_skb()
	 */
	atomic_t	dataref;

	/* must be last field, see pskb_expand_head() */
	skb_frag_t	frags[MAX_SKB_FRAGS];
};

/* We divide dataref into two halves.  The higher 16 bits hold references
 * to the payload part of skb->data.  The lower 16 bits hold references to
 * the entire skb->data.  A clone of a headerless skb holds the length of
 * the header in skb->hdr_len.
 *
 * All users must obey the rule that the skb->data reference count must be
 * greater than or equal to the payload reference count.
 *
 * Holding a reference to the payload part means that the user does not
 * care about modifications to the header part of skb->data.
 */
#define SKB_DATAREF_SHIFT 16
#define SKB_DATAREF_MASK ((1 << SKB_DATAREF_SHIFT) - 1)


enum {
	SKB_FCLONE_UNAVAILABLE,
	SKB_FCLONE_ORIG,
	SKB_FCLONE_CLONE,
};

enum {
	SKB_GSO_TCPV4 = 1 << 0,
	SKB_GSO_UDP = 1 << 1,

	/* This indicates the skb is from an untrusted source. */
	SKB_GSO_DODGY = 1 << 2,

	/* This indicates the tcp segment has CWR set. */
	SKB_GSO_TCP_ECN = 1 << 3,

	SKB_GSO_TCPV6 = 1 << 4,
};


/** 
 *	struct sk_buff - socket buffer
 *	@next: Next buffer in list
 *	@prev: Previous buffer in list
 *	@sk: Socket we are owned by
 *	@tstamp: Time we arrived
 *	@dev: Device we arrived on/are leaving by
 *	@h: Transport layer header
 *	@nh: Network layer header
 *	@mac: Link layer header
 *	@dst: destination entry
 *	@sp: the security path, used for xfrm
 *	@cb: Control buffer. Free for use by every layer. Put private vars here
 *	@len: Length of actual data
 *	@data_len: Data length
 *	@mac_len: Length of link layer header
 *	@hdr_len: writable header length of cloned skb
 *	@csum: Checksum (must include start/offset pair)
 *	@csum_start: Offset from skb->head where checksumming should start
 *	@csum_offset: Offset from csum_start where checksum should be stored
 *	@ignore_df: allow local fragmentation
 *	@cloned: Head may be cloned (check refcnt to be sure)
 *	@nohdr: Payload reference only, must not modify header
 *	@pkt_type: Packet class
 *	@fclone: skbuff clone status
 *	@ip_summed: Driver fed us an IP checksum
 *	@priority: Packet queueing priority
 *	@users: User count - see {datagram,tcp}.c
 *	@protocol: Packet protocol from driver
 *	@truesize: Buffer size 
 *	@head: Head of buffer
 *	@data: Data head pointer
 *	@tail: Tail pointer
 *	@end: End pointer
 *	@destructor: Destruct function
 *	@mark: Generic packet mark
 *	@nfct: Associated connection, if any
 *	@ipvs_property: skbuff is owned by ipvs
 *	@nf_trace: netfilter packet trace flag
 *	@nfctinfo: Relationship of this skb to the connection
 *	@nfct_reasm: netfilter conntrack re-assembly pointer
 *	@nf_bridge: Saved data about a bridged frame - see br_netfilter.c
 *	@tc_index: Traffic control index
 *	@tc_verd: traffic control verdict
 *	@dma_cookie: a cookie to one of several possible DMA operations
 *		done by skb DMA functions
 *	@secmark: security marking
 *	@vlan_tci: vlan tag control information
 */

struct sk_buff {
	/* These two members must be first. */
	struct sk_buff		*next;
	struct sk_buff		*prev;

	ktime_t			tstamp;

	struct sock		*sk;
	struct net_device	*dev;
	int			iif;
	/* 4 byte hole on 64 bit*/

	union {
		struct tcphdr	*th;
		struct iphdr	*ipiph;
		struct ipv6hdr	*ipv6h;
		unsigned char	*raw;
	} h;

	union {
		struct iphdr	*iph;
		struct ipv6hdr	*ipv6h;
		unsigned char	*raw;
	} nh;

	union {
	  	unsigned char 	*raw;
	} mac;

	struct  dst_entry	*dst;
	struct	sec_path	*sp;

	/*
	 * This is the control buffer. It is free to use for every
	 * layer. Please put your private variables there. If you
	 * want to keep them across layers you have to do a skb_clone()
	 * first. This is owned by whoever has the skb queued ATM.
	 */
	char			cb[48] __aligned(8);

	unsigned int		len,
				data_len;
	__u16			mac_len,
				hdr_len;
	union {
		__wsum		csum;
		struct {
			__u16	csum_start;
			__u16	csum_offset;
		};
	};
	__u32			priority;
	__u8			ignore_df:1,
				cloned:1,
				ip_summed:2,
				nohdr:1,
				nfctinfo:3;
	__u8			pkt_type:3,
#if defined(CONFIG_IP_VS) || defined(CONFIG_IP_VS_MODULE)
				ipvs_property:1,
#endif
				peeked:1,
#if defined(CONFIG_NETFILTER_XT_TARGET_TRACE) || \
    defined(CONFIG_NETFILTER_XT_TARGET_TRACE_MODULE)
				nf_trace:1,
#endif
				fclone:2;
	__be16			protocol;

	void			(*destructor)(struct sk_buff *skb);
#ifdef CONFIG_NETFILTER
	struct nf_conntrack	*nfct;
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	struct sk_buff		*nfct_reasm;
#endif
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
	unsigned char		imq_flags;
	struct nf_info		*nf_info;
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	struct nf_bridge_info	*nf_bridge;
#endif
#endif /* CONFIG_NETFILTER */
#ifdef CONFIG_NET_SCHED
	__u16			tc_index;	/* traffic control index */
#ifdef CONFIG_NET_CLS_ACT
	__u16			tc_verd;	/* traffic control verdict */
#endif
#endif
#ifdef CONFIG_IPV6_NDISC_NODETYPE
	__u8			ndisc_nodetype:2;
#endif
	/* 14 bit hole */
#ifdef CONFIG_NET_DMA
	dma_cookie_t		dma_cookie;
#endif
#ifdef CONFIG_NETWORK_SECMARK
	__u32			secmark;
#endif
	__u32			mark;
	__u32			avail_size;

	/* These elements must be at the end, see alloc_skb() for details.  */

	unsigned int		truesize;
	atomic_t		users;

	__u16                   vlan_tci;
	unsigned char		*head,
				*data,
				*tail,
				*end;
};

#ifdef __KERNEL__
/*
 *	Handling routines are only of interest to the kernel
 */
#include <linux/slab.h>
#include <asm/system.h>

extern void kfree_skb(struct sk_buff *skb);
extern void	       __kfree_skb(struct sk_buff *skb);
extern struct sk_buff *__alloc_skb(unsigned int size,
				   gfp_t priority, int fclone, int node);
static __inline__ struct sk_buff *alloc_skb(unsigned int size,
					gfp_t priority)
{
	return __alloc_skb(size, priority, 0, -1);
}

static __inline__ struct sk_buff *alloc_skb_fclone(unsigned int size,
					       gfp_t priority)
{
	return __alloc_skb(size, priority, 1, -1);
}

extern struct sk_buff *skb_morph(struct sk_buff *dst, struct sk_buff *src);
extern struct sk_buff *skb_clone(struct sk_buff *skb,
				 gfp_t priority);
extern struct sk_buff *skb_copy(const struct sk_buff *skb,
				gfp_t priority);
extern struct sk_buff *__pskb_copy(struct sk_buff *skb,
				 int headroom, gfp_t gfp_mask);

extern int	       pskb_expand_head(struct sk_buff *skb,
					int nhead, int ntail,
					gfp_t gfp_mask);
extern struct sk_buff *skb_realloc_headroom(struct sk_buff *skb,
					    unsigned int headroom);
extern struct sk_buff *skb_copy_expand(const struct sk_buff *skb,
				       int newheadroom, int newtailroom,
				       gfp_t priority);
extern int	       skb_pad(struct sk_buff *skb, int pad);
#define dev_kfree_skb(a)	kfree_skb(a)
extern int skb_append_datato_frags(struct sock *sk, struct sk_buff *skb,
			int getfrag(void *from, char *to, int offset,
			int len,int odd, struct sk_buff *skb),
			void *from, int length);

struct skb_seq_state {
	__u32		lower_offset;
	__u32		upper_offset;
	__u32		frag_idx;
	__u32		stepped_offset;
	struct sk_buff	*root_skb;
	struct sk_buff	*cur_skb;
	__u8		*frag_data;
};

extern unsigned int   skb_find_text(struct sk_buff *skb, unsigned int from,
				    unsigned int to, struct ts_config *config,
				    struct ts_state *state);

/* Internal */
#define skb_shinfo(SKB)		((struct skb_shared_info *)((SKB)->end))

/**
 *	skb_queue_empty - check if a queue is empty
 *	@list: queue head
 *
 *	Returns true if the queue is empty, false otherwise.
 */
static __inline__ int skb_queue_empty(const struct sk_buff_head *list)
{
	return list->next == (struct sk_buff *)list;
}

/**
 *     skb_queue_is_last - check if skb is the last entry in the queue
 *     @list: queue head
 *     @skb: buffer
 *
 *     Returns true if @skb is the last buffer on the list.
 */
static __inline__ bool skb_queue_is_last(const struct sk_buff_head *list,
                                    const struct sk_buff *skb)
{
       return (skb->next == (struct sk_buff *) list);
}

/**
 *     skb_queue_next - return the next packet in the queue
 *     @list: queue head
 *     @skb: current buffer
 *
 *     Return the next packet in @list after @skb.  It is only valid to
 *     call this if skb_queue_is_last() evaluates to false.
 */
static __inline__ struct sk_buff *skb_queue_next(const struct sk_buff_head *list,
                                            const struct sk_buff *skb)
{
       /* This BUG_ON may seem severe, but if we just return then we
        * are going to dereference garbage.
        */
       BUG_ON(skb_queue_is_last(list, skb));
       return skb->next;
}

/**
 *	skb_get - reference buffer
 *	@skb: buffer to reference
 *
 *	Makes another reference to a socket buffer and returns a pointer
 *	to the buffer.
 */
static __inline__ struct sk_buff *skb_get(struct sk_buff *skb)
{
	atomic_inc(&skb->users);
	return skb;
}

/*
 * If users == 1, we are the only owner and are can avoid redundant
 * atomic change.
 */

/**
 *	skb_cloned - is the buffer a clone
 *	@skb: buffer to check
 *
 *	Returns true if the buffer was generated with skb_clone() and is
 *	one of multiple shared copies of the buffer. Cloned buffers are
 *	shared data so must not be written to under normal circumstances.
 */
static __inline__ int skb_cloned(const struct sk_buff *skb)
{
	return skb->cloned &&
	       (atomic_read(&skb_shinfo(skb)->dataref) & SKB_DATAREF_MASK) != 1;
}

static __inline__ int skb_unclone(struct sk_buff *skb, gfp_t pri)
{
	might_sleep_if(pri & __GFP_WAIT);

	if (skb_cloned(skb))
		return pskb_expand_head(skb, 0, 0, pri);

	return 0;
}

/**
 *	skb_header_cloned - is the header a clone
 *	@skb: buffer to check
 *
 *	Returns true if modifying the header part of the buffer requires
 *	the data to be copied.
 */
static __inline__ int skb_header_cloned(const struct sk_buff *skb)
{
	int dataref;

	if (!skb->cloned)
		return 0;

	dataref = atomic_read(&skb_shinfo(skb)->dataref);
	dataref = (dataref & SKB_DATAREF_MASK) - (dataref >> SKB_DATAREF_SHIFT);
	return dataref != 1;
}

/**
 *	skb_header_release - release reference to header
 *	@skb: buffer to operate on
 *
 *	Drop a reference to the header part of the buffer.  This is done
 *	by acquiring a payload reference.  You must not read from the header
 *	part of skb->data after this.
 */
static __inline__ void skb_header_release(struct sk_buff *skb)
{
	BUG_ON(skb->nohdr);
	skb->nohdr = 1;
	atomic_add(1 << SKB_DATAREF_SHIFT, &skb_shinfo(skb)->dataref);
}

/**
 *	skb_shared - is the buffer shared
 *	@skb: buffer to check
 *
 *	Returns true if more than one person has a reference to this
 *	buffer.
 */
static __inline__ int skb_shared(const struct sk_buff *skb)
{
	return atomic_read(&skb->users) != 1;
}

/**
 *	skb_share_check - check if buffer is shared and if so clone it
 *	@skb: buffer to check
 *	@pri: priority for memory allocation
 *
 *	If the buffer is shared the buffer is cloned and the old copy
 *	drops a reference. A new clone with a single reference is returned.
 *	If the buffer is not shared the original buffer is returned. When
 *	being called from interrupt status or with spinlocks held pri must
 *	be GFP_ATOMIC.
 *
 *	NULL is returned on a memory allocation failure.
 */
static __inline__ struct sk_buff *skb_share_check(struct sk_buff *skb,
					      gfp_t pri)
{
	might_sleep_if(pri & __GFP_WAIT);
	if (skb_shared(skb)) {
		struct sk_buff *nskb = skb_clone(skb, pri);
		kfree_skb(skb);
		skb = nskb;
	}
	return skb;
}

/*
 *	Copy shared buffers into a new sk_buff. We effectively do COW on
 *	packets to handle cases where we have a local reader and forward
 *	and a couple of other messy ones. The normal one is tcpdumping
 *	a packet thats being forwarded.
 */

/**
 *	skb_unshare - make a copy of a shared buffer
 *	@skb: buffer to check
 *	@pri: priority for memory allocation
 *
 *	If the socket buffer is a clone then this function creates a new
 *	copy of the data, drops a reference count on the old copy and returns
 *	the new copy with the reference count at 1. If the buffer is not a clone
 *	the original buffer is returned. When called with a spinlock held or
 *	from interrupt state @pri must be %GFP_ATOMIC
 *
 *	%NULL is returned on a memory allocation failure.
 */
static __inline__ struct sk_buff *skb_unshare(struct sk_buff *skb,
					  gfp_t pri)
{
	might_sleep_if(pri & __GFP_WAIT);
	if (skb_cloned(skb)) {
		struct sk_buff *nskb = skb_copy(skb, pri);
		kfree_skb(skb);	/* Free our shared copy */
		skb = nskb;
	}
	return skb;
}

/**
 *	skb_peek
 *	@list_: list to peek at
 *
 *	Peek an &sk_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the head element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
static __inline__ struct sk_buff *skb_peek(struct sk_buff_head *list_)
{
	struct sk_buff *list = ((struct sk_buff *)list_)->next;
	if (list == (struct sk_buff *)list_)
		list = NULL;
	return list;
}

/**
 *	skb_peek_tail
 *	@list_: list to peek at
 *
 *	Peek an &sk_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the tail element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
static __inline__ struct sk_buff *skb_peek_tail(struct sk_buff_head *list_)
{
	struct sk_buff *list = ((struct sk_buff *)list_)->prev;
	if (list == (struct sk_buff *)list_)
		list = NULL;
	return list;
}

/**
 *	skb_queue_len	- get queue length
 *	@list_: list to measure
 *
 *	Return the length of an &sk_buff queue.
 */
static __inline__ __u32 skb_queue_len(const struct sk_buff_head *list_)
{
	return list_->qlen;
}

/**
 *	__skb_queue_head_init - initialize non-spinlock portions of sk_buff_head
 *	@list: queue to initialize
 *
 *	This initializes only the list and queue length aspects of
 *	an sk_buff_head object.  This allows to initialize the list
 *	aspects of an sk_buff_head without reinitializing things like
 *	the spinlock.  It can also be used for on-stack sk_buff_head
 *	objects where the spinlock is known to not be used.
 */
static __inline__ void __skb_queue_head_init(struct sk_buff_head *list)
{
	list->prev = list->next = (struct sk_buff *)list;
	list->qlen = 0;
}

/*
 * This function creates a split out lock class for each invocation;
 * this is needed for now since a whole lot of users of the skb-queue
 * infrastructure in drivers have different locking usage (in hardirq)
 * than the networking core (in softirq only). In the long run either the
 * network layer or drivers should need annotation to consolidate the
 * main types of usage into 3 classes.
 */
static __inline__ void skb_queue_head_init(struct sk_buff_head *list)
{
	spin_lock_init(&list->lock);
	__skb_queue_head_init(list);
}

static __inline__ void skb_queue_head_init_class(struct sk_buff_head *list,
		struct lock_class_key *class)
{
	skb_queue_head_init(list);
	lockdep_set_class(&list->lock, class);
}

/*
 *	Insert a packet on a list.
 */
extern void        skb_insert(struct sk_buff *old, struct sk_buff *newsk, struct sk_buff_head *list);
static __inline__ void __skb_insert(struct sk_buff *newsk,
				struct sk_buff *prev, struct sk_buff *next,
				struct sk_buff_head *list)
{
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
	list->qlen++;
}

static __inline__ void __skb_queue_splice(const struct sk_buff_head *list,
				      struct sk_buff *prev,
				      struct sk_buff *next)
{
	struct sk_buff *first = list->next;
	struct sk_buff *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 *	skb_queue_splice - join two skb lists, this is designed for stacks
 *	@list: the new list to add
 *	@head: the place to add it in the first list
 */
static __inline__ void skb_queue_splice(const struct sk_buff_head *list,
				    struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, (struct sk_buff *) head, head->next);
		head->qlen += list->qlen;
	}
}

/**
 *	skb_queue_splice - join two skb lists and reinitialise the emptied list
 *	@list: the new list to add
 *	@head: the place to add it in the first list
 *
 *	The list at @list is reinitialised
 */
static __inline__ void skb_queue_splice_init(struct sk_buff_head *list,
					 struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, (struct sk_buff *) head, head->next);
		head->qlen += list->qlen;
		__skb_queue_head_init(list);
	}
}

/**
 *	skb_queue_splice_tail - join two skb lists, each list being a queue
 *	@list: the new list to add
 *	@head: the place to add it in the first list
 */
static __inline__ void skb_queue_splice_tail(const struct sk_buff_head *list,
					 struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, head->prev, (struct sk_buff *) head);
		head->qlen += list->qlen;
	}
}

/**
 *	skb_queue_splice_tail - join two skb lists and reinitialise the emptied list
 *	@list: the new list to add
 *	@head: the place to add it in the first list
 *
 *	Each of the lists is a queue.
 *	The list at @list is reinitialised
 */
static __inline__ void skb_queue_splice_tail_init(struct sk_buff_head *list,
					      struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, head->prev, (struct sk_buff *) head);
		head->qlen += list->qlen;
		__skb_queue_head_init(list);
	}
}

/*
 *	Insert an sk_buff at the start of a list.
 *
 *	The "__skb_xxxx()" functions are the non-atomic ones that
 *	can only be called with interrupts disabled.
 */

/**
 *	__skb_queue_after - queue a buffer at the list head
 *	@list: list to use
 *	@prev: place after this buffer
 *	@newsk: buffer to queue
 *
 *	Queue a buffer int the middle of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
static __inline__ void __skb_queue_after(struct sk_buff_head *list,
				     struct sk_buff *prev,
				     struct sk_buff *newsk)
{
	__skb_insert(newsk, prev, prev->next, list);
}

extern void skb_append(struct sk_buff *old, struct sk_buff *newsk,
		       struct sk_buff_head *list);

/**
 *	__skb_queue_head - queue a buffer at the list head
 *	@list: list to use
 *	@newsk: buffer to queue
 *
 *	Queue a buffer at the start of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
extern void skb_queue_head(struct sk_buff_head *list, struct sk_buff *newsk);
static __inline__ void __skb_queue_head(struct sk_buff_head *list,
				    struct sk_buff *newsk)
{
	__skb_queue_after(list, (struct sk_buff *)list, newsk);
}

static __inline__ void __skb_queue_before(struct sk_buff_head *list,
				      struct sk_buff *next,
				      struct sk_buff *newsk)
{
	__skb_insert(newsk, next->prev, next, list);
}

/**
 *	__skb_queue_tail - queue a buffer at the list tail
 *	@list: list to use
 *	@newsk: buffer to queue
 *
 *	Queue a buffer at the end of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
extern void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk);
static __inline__ void __skb_queue_tail(struct sk_buff_head *list,
				   struct sk_buff *newsk)
{
	__skb_queue_before(list, (struct sk_buff *)list, newsk);
}


/*
 * remove sk_buff from list. _Must_ be called atomically, and with
 * the list known..
 */
extern void	   skb_unlink(struct sk_buff *skb, struct sk_buff_head *list);
static __inline__ void __skb_unlink(struct sk_buff *skb, struct sk_buff_head *list)
{
	struct sk_buff *next, *prev;

	list->qlen--;
	next	   = skb->next;
	prev	   = skb->prev;
	skb->next  = skb->prev = NULL;
	next->prev = prev;
	prev->next = next;
}

/**
 *	__skb_dequeue - remove from the head of the queue
 *	@list: list to dequeue from
 *
 *	Remove the head of the list. This function does not take any locks
 *	so must be used with appropriate locks held only. The head item is
 *	returned or %NULL if the list is empty.
 */
extern struct sk_buff *skb_dequeue(struct sk_buff_head *list);
static __inline__ struct sk_buff *__skb_dequeue(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek(list);
	if (skb)
		__skb_unlink(skb, list);
	return skb;
}

/* XXX: more streamlined implementation */

/**
 *	__skb_dequeue_tail - remove from the tail of the queue
 *	@list: list to dequeue from
 *
 *	Remove the tail of the list. This function does not take any locks
 *	so must be used with appropriate locks held only. The tail item is
 *	returned or %NULL if the list is empty.
 */
extern struct sk_buff *skb_dequeue_tail(struct sk_buff_head *list);
static __inline__ struct sk_buff *__skb_dequeue_tail(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek_tail(list);
	if (skb)
		__skb_unlink(skb, list);
	return skb;
}


static __inline__ int skb_is_nonlinear(const struct sk_buff *skb)
{
	return skb->data_len;
}

static __inline__ unsigned int skb_headlen(const struct sk_buff *skb)
{
	return skb->len - skb->data_len;
}

static __inline__ int skb_pagelen(const struct sk_buff *skb)
{
	int i, len = 0;

	for (i = (int)skb_shinfo(skb)->nr_frags - 1; i >= 0; i--)
		len += skb_shinfo(skb)->frags[i].size;
	return len + skb_headlen(skb);
}

static __inline__ bool skb_has_frags(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->nr_frags;
}

static __inline__ void skb_fill_page_desc(struct sk_buff *skb, int i,
				      struct page *page, int off, int size)
{
	skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

	frag->page		  = page;
	frag->page_offset	  = off;
	frag->size		  = size;
	skb_shinfo(skb)->nr_frags = i + 1;
}

#define SKB_PAGE_ASSERT(skb) 	BUG_ON(skb_shinfo(skb)->nr_frags)
#define SKB_FRAG_ASSERT(skb) 	BUG_ON(skb_shinfo(skb)->frag_list)
#define SKB_LINEAR_ASSERT(skb)  BUG_ON(skb_is_nonlinear(skb))

static __inline__ unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static __inline__ void skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data;
}

static __inline__ void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb->tail = skb->data + offset;
}

/*
 *	Add data to an sk_buff
 */
extern unsigned char *skb_put(struct sk_buff *skb, unsigned int len);
static __inline__ unsigned char *__skb_put(struct sk_buff *skb, unsigned int len)
{
	unsigned char *tmp = skb->tail;
	SKB_LINEAR_ASSERT(skb);
	skb->tail += len;
	skb->len  += len;
	return tmp;
}

extern unsigned char *skb_push(struct sk_buff *skb, unsigned int len);
static __inline__ unsigned char *__skb_push(struct sk_buff *skb, unsigned int len)
{
	skb->data -= len;
	skb->len  += len;
	return skb->data;
}

extern unsigned char *skb_pull(struct sk_buff *skb, unsigned int len);
static __inline__ unsigned char *__skb_pull(struct sk_buff *skb, unsigned int len)
{
	skb->len -= len;
	BUG_ON(skb->len < skb->data_len);
	return skb->data += len;
}

static __inline__ unsigned char *skb_pull_inline(struct sk_buff *skb, unsigned int len)
{
	return unlikely(len > skb->len) ? NULL : __skb_pull(skb, len);
}

extern unsigned char *__pskb_pull_tail(struct sk_buff *skb, int delta);
static __inline__ unsigned char *__pskb_pull(struct sk_buff *skb, unsigned int len)
{
	if (len > skb_headlen(skb) &&
	    !__pskb_pull_tail(skb, len - skb_headlen(skb)))
		return NULL;
	skb->len -= len;
	return skb->data += len;
}

static __inline__ unsigned char *pskb_pull(struct sk_buff *skb, unsigned int len)
{
	return unlikely(len > skb->len) ? NULL : __pskb_pull(skb, len);
}

static __inline__ int pskb_may_pull(struct sk_buff *skb, unsigned int len)
{
	if (likely(len <= skb_headlen(skb)))
		return 1;
	if (unlikely(len > skb->len))
		return 0;
	return __pskb_pull_tail(skb, len - skb_headlen(skb)) != NULL;
}

/**
 *	skb_headroom - bytes at buffer head
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the head of an &sk_buff.
 */
static __inline__ unsigned int skb_headroom(const struct sk_buff *skb)
{
	return skb->data - skb->head;
}

/**
 *	skb_tailroom - bytes at buffer end
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the tail of an sk_buff
 */
static __inline__ int skb_tailroom(const struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? 0 : skb->end - skb->tail;
}

/**
 *	skb_availroom - bytes at buffer end
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the tail of an sk_buff
 *	allocated by sk_stream_alloc()
 */
static __inline__ int skb_availroom(const struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? 0 : skb->avail_size - skb->len;
}

/**
 *	skb_reserve - adjust headroom
 *	@skb: buffer to alter
 *	@len: bytes to move
 *
 *	Increase the headroom of an empty &sk_buff by reducing the tail
 *	room. This is only allowed for an empty buffer.
 */
static __inline__ void skb_reserve(struct sk_buff *skb, int len)
{
	skb->data += len;
	skb->tail += len;
}

static __inline__ unsigned char *skb_transport_header(const struct sk_buff *skb)
{
	return skb->h.raw;
}

static __inline__ void skb_reset_transport_header(struct sk_buff *skb)
{
	skb->h.raw = skb->data;
}

static __inline__ void skb_set_transport_header(struct sk_buff *skb,
					    const int offset)
{
	skb->h.raw = skb->data + offset;
}

static __inline__ int skb_transport_offset(const struct sk_buff *skb)
{
	return skb->h.raw - skb->data;
}

static __inline__ unsigned char *skb_network_header(const struct sk_buff *skb)
{
	return skb->nh.raw;
}

static __inline__ void skb_reset_network_header(struct sk_buff *skb)
{
	skb->nh.raw = skb->data;
}

static __inline__ void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb->nh.raw = skb->data + offset;
}

static __inline__ int skb_network_offset(const struct sk_buff *skb)
{
	return skb->nh.raw - skb->data;
}

static __inline__ unsigned char *skb_mac_header(const struct sk_buff *skb)
{
	return skb->mac.raw;
}

static __inline__ int skb_mac_header_was_set(const struct sk_buff *skb)
{
	return skb->mac.raw != NULL;
}

static __inline__ void skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac.raw = skb->data;
}

static __inline__ void skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb->mac.raw = skb->data + offset;
}

/*
 * CPUs often take a performance hit when accessing unaligned memory
 * locations. The actual performance hit varies, it can be small if the
 * hardware handles it or large if we have to take an exception and fix it
 * in software.
 *
 * Since an ethernet header is 14 bytes network drivers often end up with
 * the IP header at an unaligned offset. The IP header can be aligned by
 * shifting the start of the packet by 2 bytes. Drivers should do this
 * with:
 *
 * skb_reserve(NET_IP_ALIGN);
 *
 * The downside to this alignment of the IP header is that the DMA is now
 * unaligned. On some architectures the cost of an unaligned DMA is high
 * and this cost outweighs the gains made by aligning the IP header.
 * 
 * Since this trade off varies between architectures, we allow NET_IP_ALIGN
 * to be overridden.
 */
#ifndef NET_IP_ALIGN
#define NET_IP_ALIGN	2
#endif

/*
 * The networking layer reserves some headroom in skb data (via
 * dev_alloc_skb). This is used to avoid having to reallocate skb data when
 * the header has to grow. In the default case, if the header has to grow
 * 32 bytes or less we avoid the reallocation.
 *
 * Unfortunately this headroom changes the DMA alignment of the resulting
 * network packet. As for NET_IP_ALIGN, this unaligned DMA is expensive
 * on some architectures. An architecture can override this value,
 * perhaps setting it to a cacheline in size (since that will maintain
 * cacheline alignment of the DMA). It must be a power of 2.
 *
 * Various parts of the networking layer expect at least 32 bytes of
 * headroom, you should not reduce this.
 *
 * Using max(32, L1_CACHE_BYTES) makes sense (especially with RPS)
 * to reduce average number of cache lines per packet.
 * get_rps_cpus() for example only access one 64 bytes aligned block :
 * NET_IP_ALIGN(2) + ethernet_header(14) + IP_header(20/40) + ports(8)
 */
#ifndef NET_SKB_PAD
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3352) ||  defined (CONFIG_RALINK_RT3883) || \
    defined (CONFIG_RALINK_RT5350) || defined (CONFIG_RALINK_RT6855) || defined (CONFIG_RALINK_MT7620)
/* ralink depended hacks */
#define NET_SKB_PAD		96
#define NET_SKB_PAD_ORIG	max(64, L1_CACHE_BYTES)
#else
#define NET_SKB_PAD		max(32, L1_CACHE_BYTES)
#define NET_SKB_PAD_ORIG	NET_SKB_PAD
#endif
#endif

extern int ___pskb_trim(struct sk_buff *skb, unsigned int len);

static __inline__ void __skb_trim(struct sk_buff *skb, unsigned int len)
{
	if (unlikely(skb_is_nonlinear(skb))) {
		WARN_ON(1);
		return;
	}
	skb->len  = len;
	skb->tail = skb->data + len;
}

/**
 *	skb_trim - remove end from a buffer
 *	@skb: buffer to alter
 *	@len: new length
 *
 *	Cut the length of a buffer down by removing data from the tail. If
 *	the buffer is already under the length specified it is not modified.
 *	The skb must be linear.
 */
static __inline__ void skb_trim(struct sk_buff *skb, unsigned int len)
{
	if (skb->len > len)
		__skb_trim(skb, len);
}


static __inline__ int __pskb_trim(struct sk_buff *skb, unsigned int len)
{
	if (skb->data_len)
		return ___pskb_trim(skb, len);
	__skb_trim(skb, len);
	return 0;
}

static __inline__ int pskb_trim(struct sk_buff *skb, unsigned int len)
{
	return (len < skb->len) ? __pskb_trim(skb, len) : 0;
}

/**
 *	pskb_trim_unique - remove end from a paged unique (not cloned) buffer
 *	@skb: buffer to alter
 *	@len: new length
 *
 *	This is identical to pskb_trim except that the caller knows that
 *	the skb is not cloned so we should never get an error due to out-
 *	of-memory.
 */
static __inline__ void pskb_trim_unique(struct sk_buff *skb, unsigned int len)
{
	int err = pskb_trim(skb, len);
	BUG_ON(err);
}

/**
 *	skb_orphan - orphan a buffer
 *	@skb: buffer to orphan
 *
 *	If a buffer currently has an owner then we call the owner's
 *	destructor function and make the @skb unowned. The buffer continues
 *	to exist but is no longer charged to its former owner.
 */
static __inline__ void skb_orphan(struct sk_buff *skb)
{
	if (skb->destructor)
		skb->destructor(skb);
	skb->destructor = NULL;
	skb->sk		= NULL;
}

/**
 *	__skb_queue_purge - empty a list
 *	@list: list to empty
 *
 *	Delete all buffers on an &sk_buff list. Each buffer is removed from
 *	the list and one reference dropped. This function does not take the
 *	list lock and the caller must hold the relevant locks to use it.
 */
extern void skb_queue_purge(struct sk_buff_head *list);
static __inline__ void __skb_queue_purge(struct sk_buff_head *list)
{
	struct sk_buff *skb;
	while ((skb = __skb_dequeue(list)) != NULL)
		kfree_skb(skb);
}

/**
 *	__dev_alloc_skb - allocate an skbuff for receiving
 *	@length: length to allocate
 *	@gfp_mask: get_free_pages mask, passed to alloc_skb
 *
 *	Allocate a new &sk_buff and assign it a usage count of one. The
 *	buffer has unspecified headroom built in. Users should allocate
 *	the headroom they think they need without accounting for the
 *	built in space. The built in space is used for optimisations.
 *
 *	%NULL is returned if there is no free memory.
 */
static __inline__ struct sk_buff *__dev_alloc_skb(unsigned int length,
					      gfp_t gfp_mask)
{
	struct sk_buff *skb = alloc_skb(length + NET_SKB_PAD, gfp_mask);
	if (likely(skb))
		skb_reserve(skb, NET_SKB_PAD);
	return skb;
}

/**
 *	dev_alloc_skb - allocate an skbuff for receiving
 *	@length: length to allocate
 *
 *	Allocate a new &sk_buff and assign it a usage count of one. The
 *	buffer has unspecified headroom built in. Users should allocate
 *	the headroom they think they need without accounting for the
 *	built in space. The built in space is used for optimisations.
 *
 *	%NULL is returned if there is no free memory. Although this function
 *	allocates memory it can be called from an interrupt.
 */
static __inline__ struct sk_buff *dev_alloc_skb(unsigned int length)
{
	return __dev_alloc_skb(length, GFP_ATOMIC);
}

extern struct sk_buff *__netdev_alloc_skb(struct net_device *dev,
		unsigned int length, gfp_t gfp_mask);

/**
 *	netdev_alloc_skb - allocate an skbuff for rx on a specific device
 *	@dev: network device to receive on
 *	@length: length to allocate
 *
 *	Allocate a new &sk_buff and assign it a usage count of one. The
 *	buffer has unspecified headroom built in. Users should allocate
 *	the headroom they think they need without accounting for the
 *	built in space. The built in space is used for optimisations.
 *
 *	%NULL is returned if there is no free memory. Although this function
 *	allocates memory it can be called from an interrupt.
 */
static __inline__ struct sk_buff *netdev_alloc_skb(struct net_device *dev,
		unsigned int length)
{
	return __netdev_alloc_skb(dev, length, GFP_ATOMIC);
}

static __inline__ struct sk_buff *netdev_alloc_skb_ip_align(struct net_device *dev,
		unsigned int length)
{
	struct sk_buff *skb = netdev_alloc_skb(dev, length + NET_IP_ALIGN);

	if (NET_IP_ALIGN && skb)
		skb_reserve(skb, NET_IP_ALIGN);
	return skb;
}

static __inline__ int __skb_cow(struct sk_buff *skb, unsigned int headroom,
                            int cloned)
{
        int delta = 0;

        if (headroom > skb_headroom(skb))
                delta = headroom - skb_headroom(skb);

        if (delta || cloned)
		return pskb_expand_head(skb, ALIGN(delta, NET_SKB_PAD_ORIG), 0,
                                        GFP_ATOMIC);
        return 0;
}

static __inline__ struct sk_buff *pskb_copy(struct sk_buff *skb,
					gfp_t gfp_mask)
{
	return __pskb_copy(skb, skb_headroom(skb), gfp_mask);
}

/**
 *	skb_clone_writable - is the header of a clone writable
 *	@skb: buffer to check
 *	@len: length up to which to write
 *
 *	Returns true if modifying the header part of the cloned buffer
 *	does not requires the data to be copied.
 */
static __inline__ int skb_clone_writable(struct sk_buff *skb, unsigned int len)
{
	return !skb_header_cloned(skb) &&
	       skb_headroom(skb) + len <= skb->hdr_len;
}

/**
 *	skb_cow - copy header of skb when it is required
 *	@skb: buffer to cow
 *	@headroom: needed headroom
 *
 *	If the skb passed lacks sufficient headroom or its data part
 *	is shared, data is reallocated. If reallocation fails, an error
 *	is returned and original skb is not changed.
 *
 *	The result is skb with writable area skb->head...skb->tail
 *	and at least @headroom of space at head.
 */
static __inline__ int skb_cow(struct sk_buff *skb, unsigned int headroom)
{
	int delta = (headroom > NET_SKB_PAD ? headroom : NET_SKB_PAD) -
			skb_headroom(skb);

	if (delta < 0)
		delta = 0;

	if (delta || skb_cloned(skb))
		return pskb_expand_head(skb, (delta + (NET_SKB_PAD-1)) &
				~(NET_SKB_PAD-1), 0, GFP_ATOMIC);
	return 0;
}

static __inline__ int skb_cow_head(struct sk_buff *skb, unsigned int headroom)
{
        return __skb_cow(skb, headroom, skb_header_cloned(skb));
}

/**
 *	skb_padto	- pad an skbuff up to a minimal size
 *	@skb: buffer to pad
 *	@len: minimal length
 *
 *	Pads up a buffer to ensure the trailing bytes exist and are
 *	blanked. If the buffer already contains sufficient data it
 *	is untouched. Otherwise it is extended. Returns zero on
 *	success. The skb is freed on error.
 */
 
static __inline__ int skb_padto(struct sk_buff *skb, unsigned int len)
{
	unsigned int size = skb->len;
	if (likely(size >= len))
		return 0;
	return skb_pad(skb, len - size);
}

static __inline__ int skb_add_data(struct sk_buff *skb,
			       char *from, int copy)
{
	const int off = skb->len;

	if (skb->ip_summed == CHECKSUM_NONE) {
		int err = 0;
		__wsum csum = csum_and_copy_from_user(from, skb_put(skb, copy),
							    copy, 0, &err);
		if (!err) {
			skb->csum = csum_block_add(skb->csum, csum, off);
			return 0;
		}
	} else if (!copy_from_user(skb_put(skb, copy), from, copy))
		return 0;

	__skb_trim(skb, off);
	return -EFAULT;
}

static __inline__ int skb_can_coalesce(struct sk_buff *skb, int i,
				   struct page *page, int off)
{
	if (i) {
		struct skb_frag_struct *frag = &skb_shinfo(skb)->frags[i - 1];

		return page == frag->page &&
		       off == frag->page_offset + frag->size;
	}
	return 0;
}

static __inline__ int __skb_linearize(struct sk_buff *skb)
{
	return __pskb_pull_tail(skb, skb->data_len) ? 0 : -ENOMEM;
}

/**
 *	skb_linearize - convert paged skb to linear one
 *	@skb: buffer to linarize
 *
 *	If there is no free memory -ENOMEM is returned, otherwise zero
 *	is returned and the old skb data released.
 */
static __inline__ int skb_linearize(struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? __skb_linearize(skb) : 0;
}

/**
 *	skb_linearize_cow - make sure skb is linear and writable
 *	@skb: buffer to process
 *
 *	If there is no free memory -ENOMEM is returned, otherwise zero
 *	is returned and the old skb data released.
 */
static __inline__ int skb_linearize_cow(struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) || skb_cloned(skb) ?
	       __skb_linearize(skb) : 0;
}

/**
 *	skb_postpull_rcsum - update checksum for received skb after pull
 *	@skb: buffer to update
 *	@start: start of data before pull
 *	@len: length of data pulled
 *
 *	After doing a pull on a received packet, you need to call this to
 *	update the CHECKSUM_COMPLETE checksum, or set ip_summed to
 *	CHECKSUM_NONE so that it can be recomputed from scratch.
 */

static __inline__ void skb_postpull_rcsum(struct sk_buff *skb,
				      const void *start, unsigned int len)
{
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->csum = csum_sub(skb->csum, csum_partial(start, len, 0));
}

unsigned char *skb_pull_rcsum(struct sk_buff *skb, unsigned int len);

/**
 *	pskb_trim_rcsum - trim received skb and update checksum
 *	@skb: buffer to trim
 *	@len: new length
 *
 *	This is exactly the same as pskb_trim except that it ensures the
 *	checksum of received packets are still valid after the operation.
 */

static __inline__ int pskb_trim_rcsum(struct sk_buff *skb, unsigned int len)
{
	if (likely(len >= skb->len))
		return 0;
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->ip_summed = CHECKSUM_NONE;
	return __pskb_trim(skb, len);
}

#define skb_queue_walk(queue, skb) \
                for (skb = (queue)->next;                                       \
                     skb != (struct sk_buff *)(queue);   \
                     skb = skb->next)

#define skb_queue_walk_safe(queue, skb, tmp)                                    \
                for (skb = (queue)->next, tmp = skb->next;                      \
                     skb != (struct sk_buff *)(queue);                          \
                     skb = tmp, tmp = skb->next)

#define skb_queue_walk_from(queue, skb)                                                \
		for (; skb != (struct sk_buff *)(queue);			\
                    skb = skb->next)

#define skb_queue_walk_from_safe(queue, skb, tmp)                              \
		for (tmp = skb->next;                                           \
                    skb != (struct sk_buff *)(queue);                          \
                    skb = tmp, tmp = skb->next)

#define skb_queue_reverse_walk(queue, skb) \
		for (skb = (queue)->prev;					\
		     skb != (struct sk_buff *)(queue);	\
		     skb = skb->prev)

#define skb_queue_reverse_walk_safe(queue, skb, tmp)				\
		for (skb = (queue)->prev, tmp = skb->prev;			\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->prev)

#define skb_queue_reverse_walk_from_safe(queue, skb, tmp)			\
		for (tmp = skb->prev;						\
		     skb != (struct sk_buff *)(queue);				\
		     skb = tmp, tmp = skb->prev)

extern struct sk_buff *__skb_recv_datagram(struct sock *sk, unsigned flags,
					   int *peeked, int *err);
extern struct sk_buff *skb_recv_datagram(struct sock *sk, unsigned flags,
					 int noblock, int *err);
extern unsigned int    datagram_poll(struct file *file, struct socket *sock,
				     struct poll_table_struct *wait);
extern int	       skb_copy_datagram_iovec(const struct sk_buff *from,
					       int offset, struct iovec *to,
					       int size);
extern int	       skb_copy_and_csum_datagram_iovec(struct sk_buff *skb,
							int hlen,
							struct iovec *iov);
extern void	       skb_free_datagram(struct sock *sk, struct sk_buff *skb);
extern void	       skb_free_datagram_locked(struct sock *sk,
						struct sk_buff *skb);
extern int	       skb_kill_datagram(struct sock *sk, struct sk_buff *skb,
					 unsigned int flags);
extern __wsum	       skb_checksum(const struct sk_buff *skb, int offset,
				    int len, __wsum csum);
extern int	       skb_copy_bits(const struct sk_buff *skb, int offset,
				     void *to, int len);
extern int	       skb_store_bits(struct sk_buff *skb, int offset,
				      const void *from, int len);
extern __wsum	       skb_copy_and_csum_bits(const struct sk_buff *skb,
					      int offset, u8 *to, int len,
					      __wsum csum);
extern void	       skb_copy_and_csum_dev(const struct sk_buff *skb, u8 *to);
extern void	       skb_split(struct sk_buff *skb,
				 struct sk_buff *skb1, const u32 len);

extern struct sk_buff *skb_segment(struct sk_buff *skb, int features);

static __inline__ void *skb_header_pointer(const struct sk_buff *skb, int offset,
				       int len, void *buffer)
{
	int hlen = skb_headlen(skb);

	if (hlen - offset >= len)
		return skb->data + offset;

	if (skb_copy_bits(skb, offset, buffer, len) < 0)
		return NULL;

	return buffer;
}

extern void skb_init(void);

/**
 *	skb_get_timestamp - get timestamp from a skb
 *	@skb: skb to get stamp from
 *	@stamp: pointer to struct timeval to store stamp in
 *
 *	Timestamps are stored in the skb as offsets to a base timestamp.
 *	This function converts the offset back to a struct timeval and stores
 *	it in stamp.
 */
static __inline__ void skb_get_timestamp(const struct sk_buff *skb, struct timeval *stamp)
{
	*stamp = ktime_to_timeval(skb->tstamp);
}

static __inline__ void __net_timestamp(struct sk_buff *skb)
{
	skb->tstamp = ktime_get_real();
}

static __inline__ ktime_t net_timedelta(ktime_t t)
{
	return ktime_sub(ktime_get_real(), t);
}


extern __sum16 __skb_checksum_complete_head(struct sk_buff *skb, int len);
extern __sum16 __skb_checksum_complete(struct sk_buff *skb);

static __inline__ int skb_csum_unnecessary(const struct sk_buff *skb)
{
	return skb->ip_summed & CHECKSUM_UNNECESSARY;
}

/**
 *	skb_checksum_complete - Calculate checksum of an entire packet
 *	@skb: packet to process
 *
 *	This function calculates the checksum over the entire packet plus
 *	the value of skb->csum.  The latter can be used to supply the
 *	checksum of a pseudo header as used by TCP/UDP.  It returns the
 *	checksum.
 *
 *	For protocols that contain complete checksums such as ICMP/TCP/UDP,
 *	this function can be used to verify that checksum on received
 *	packets.  In that case the function should return zero if the
 *	checksum is correct.  In particular, this function will return zero
 *	if skb->ip_summed is CHECKSUM_UNNECESSARY which indicates that the
 *	hardware has already verified the correctness of the checksum.
 */
static __inline__ __sum16 skb_checksum_complete(struct sk_buff *skb)
{
	return skb_csum_unnecessary(skb) ?
	       0 : __skb_checksum_complete(skb);
}

static __inline__ struct iphdr *ip_hdr(const struct sk_buff *skb)
{
        return skb->nh.iph;
}

#ifdef CONFIG_NETFILTER
extern void nf_conntrack_destroy(struct nf_conntrack *nfct);
static __inline__ void nf_conntrack_put(struct nf_conntrack *nfct)
{
	if (nfct && atomic_dec_and_test(&nfct->use))
		nf_conntrack_destroy(nfct);
}
static __inline__ void nf_conntrack_get(struct nf_conntrack *nfct)
{
	if (nfct)
		atomic_inc(&nfct->use);
}
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
static __inline__ void nf_conntrack_get_reasm(struct sk_buff *skb)
{
	if (skb)
		atomic_inc(&skb->users);
}
static __inline__ void nf_conntrack_put_reasm(struct sk_buff *skb)
{
	if (skb)
	kfree_skb(skb);
}
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
static __inline__ void nf_bridge_put(struct nf_bridge_info *nf_bridge)
{
	if (nf_bridge && atomic_dec_and_test(&nf_bridge->use))
		kfree(nf_bridge);
}
static __inline__ void nf_bridge_get(struct nf_bridge_info *nf_bridge)
{
	if (nf_bridge)
		atomic_inc(&nf_bridge->use);
}
#endif /* CONFIG_BRIDGE_NETFILTER */
static __inline__ void nf_reset(struct sk_buff *skb)
{
	nf_conntrack_put(skb->nfct);
	skb->nfct = NULL;
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	nf_conntrack_put_reasm(skb->nfct_reasm);
	skb->nfct_reasm = NULL;
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	nf_bridge_put(skb->nf_bridge);
	skb->nf_bridge = NULL;
#endif
}

/* Note: This doesn't put any conntrack and bridge info in dst. */
static __inline__ void __nf_copy(struct sk_buff *dst, const struct sk_buff *src)
{
	dst->nfct = src->nfct;
	nf_conntrack_get(src->nfct);
	dst->nfctinfo = src->nfctinfo;
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	dst->nfct_reasm = src->nfct_reasm;
	nf_conntrack_get_reasm(src->nfct_reasm);
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	dst->nf_bridge  = src->nf_bridge;
	nf_bridge_get(src->nf_bridge);
#endif
}

static __inline__ void nf_copy(struct sk_buff *dst, const struct sk_buff *src)
{
	nf_conntrack_put(dst->nfct);
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	nf_conntrack_put_reasm(dst->nfct_reasm);
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	nf_bridge_put(dst->nf_bridge);
#endif
	__nf_copy(dst, src);
}

#else /* CONFIG_NETFILTER */
static __inline__ void nf_reset(struct sk_buff *skb) {}
static __inline__ void __nf_copy(struct sk_buff *dst, const struct sk_buff *src) {}
static __inline__ void nf_copy(struct sk_buff *dst, const struct sk_buff *src) {}
#endif /* CONFIG_NETFILTER */

#ifdef CONFIG_NETWORK_SECMARK
static __inline__ void skb_copy_secmark(struct sk_buff *to, const struct sk_buff *from)
{
	to->secmark = from->secmark;
}

static __inline__ void skb_init_secmark(struct sk_buff *skb)
{
	skb->secmark = 0;
}
#else
static __inline__ void skb_copy_secmark(struct sk_buff *to, const struct sk_buff *from)
{ }

static __inline__ void skb_init_secmark(struct sk_buff *skb)
{ }
#endif

static __inline__ int skb_is_gso(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_size;
}

static __inline__ void skb_forward_csum(struct sk_buff *skb)
{
	/* Unfortunately we don't support this one.  Any brave souls? */
	if (skb->ip_summed == CHECKSUM_COMPLETE)
		skb->ip_summed = CHECKSUM_NONE;
}
#endif	/* __KERNEL__ */
#endif	/* _LINUX_SKBUFF_H */
