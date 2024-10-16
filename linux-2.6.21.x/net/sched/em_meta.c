/*
 * net/sched/em_meta.c	Metadata ematch
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Thomas Graf <tgraf@suug.ch>
 *
 * ==========================================================================
 *
 * 	The metadata ematch compares two meta objects where each object
 * 	represents either a meta value stored in the kernel or a static
 * 	value provided by userspace. The objects are not provided by
 * 	userspace itself but rather a definition providing the information
 * 	to build them. Every object is of a certain type which must be
 * 	equal to the object it is being compared to.
 *
 * 	The definition of a objects conists of the type (meta type), a
 * 	identifier (meta id) and additional type specific information.
 * 	The meta id is either TCF_META_TYPE_VALUE for values provided by
 * 	userspace or a index to the meta operations table consisting of
 * 	function pointers to type specific meta data collectors returning
 * 	the value of the requested meta value.
 *
 * 	         lvalue                                   rvalue
 * 	      +-----------+                           +-----------+
 * 	      | type: INT |                           | type: INT |
 * 	 def  | id: DEV   |                           | id: VALUE |
 * 	      | data:     |                           | data: 3   |
 * 	      +-----------+                           +-----------+
 * 	            |                                       |
 * 	            ---> meta_ops[INT][DEV](...)            |
 *	                      |                             |
 * 	            -----------                             |
 * 	            V                                       V
 * 	      +-----------+                           +-----------+
 * 	      | type: INT |                           | type: INT |
 * 	 obj  | id: DEV |                             | id: VALUE |
 * 	      | data: 2   |<--data got filled out     | data: 3   |
 * 	      +-----------+                           +-----------+
 * 	            |                                         |
 * 	            --------------> 2  equals 3 <--------------
 *
 * 	This is a simplified schema, the complexity varies depending
 * 	on the meta type. Obviously, the length of the data must also
 * 	be provided for non-numeric types.
 *
 * 	Additionaly, type dependant modifiers such as shift operators
 * 	or mask may be applied to extend the functionaliy. As of now,
 * 	the variable length type supports shifting the byte string to
 * 	the right, eating up any number of octets and thus supporting
 * 	wildcard interface name comparisons such as "ppp%" matching
 * 	ppp0..9.
 *
 * 	NOTE: Certain meta values depend on other subsystems and are
 * 	      only available if that subsytem is enabled in the kernel.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/skbuff.h>
#include <linux/random.h>
#include <linux/tc_ematch/tc_em_meta.h>
#include <net/dst.h>
#include <net/route.h>
#include <net/pkt_cls.h>
#include <net/sock.h>

struct meta_obj
{
	unsigned long		value;
	unsigned int		len;
};

struct meta_value
{
	struct tcf_meta_val	hdr;
	unsigned long		val;
	unsigned int		len;
};

struct meta_match
{
	struct meta_value	lvalue;
	struct meta_value	rvalue;
};

static inline int meta_id(struct meta_value *v)
{
	return TCF_META_ID(v->hdr.kind);
}

static inline int meta_type(struct meta_value *v)
{
	return TCF_META_TYPE(v->hdr.kind);
}

#define META_COLLECTOR(FUNC) static void meta_##FUNC(struct sk_buff *skb, \
	struct tcf_pkt_info *info, struct meta_value *v, \
	struct meta_obj *dst, int *err)

/**************************************************************************
 * System status & misc
 **************************************************************************/

META_COLLECTOR(int_random)
{
	get_random_bytes(&dst->value, sizeof(dst->value));
}

static inline unsigned long fixed_loadavg(int load)
{
	int rnd_load = load + (FIXED_1/200);
	int rnd_frac = ((rnd_load & (FIXED_1-1)) * 100) >> FSHIFT;

	return ((rnd_load >> FSHIFT) * 100) + rnd_frac;
}

META_COLLECTOR(int_loadavg_0)
{
	dst->value = fixed_loadavg(avenrun[0]);
}

META_COLLECTOR(int_loadavg_1)
{
	dst->value = fixed_loadavg(avenrun[1]);
}

META_COLLECTOR(int_loadavg_2)
{
	dst->value = fixed_loadavg(avenrun[2]);
}

/**************************************************************************
 * Device names & indices
 **************************************************************************/

static inline int int_dev(struct net_device *dev, struct meta_obj *dst)
{
	if (unlikely(dev == NULL))
		return -1;

	dst->value = dev->ifindex;
	return 0;
}

static inline int var_dev(struct net_device *dev, struct meta_obj *dst)
{
	if (unlikely(dev == NULL))
		return -1;

	dst->value = (unsigned long) dev->name;
	dst->len = strlen(dev->name);
	return 0;
}

META_COLLECTOR(int_dev)
{
	*err = int_dev(skb->dev, dst);
}

META_COLLECTOR(var_dev)
{
	*err = var_dev(skb->dev, dst);
}

/**************************************************************************
 * skb attributes
 **************************************************************************/

META_COLLECTOR(int_priority)
{
	dst->value = skb->priority;
}

META_COLLECTOR(int_protocol)
{
	/* Let userspace take care of the byte ordering */
	dst->value = skb->protocol;
}

META_COLLECTOR(int_pkttype)
{
	dst->value = skb->pkt_type;
}

META_COLLECTOR(int_pktlen)
{
	dst->value = skb->len;
}

META_COLLECTOR(int_datalen)
{
	dst->value = skb->data_len;
}

META_COLLECTOR(int_maclen)
{
	dst->value = skb->mac_len;
}

/**************************************************************************
 * Netfilter
 **************************************************************************/

META_COLLECTOR(int_mark)
{
	dst->value = skb->mark;
}

/**************************************************************************
 * Traffic Control
 **************************************************************************/

META_COLLECTOR(int_tcindex)
{
	dst->value = skb->tc_index;
}

/**************************************************************************
 * Routing
 **************************************************************************/

META_COLLECTOR(int_rtclassid)
{
	if (unlikely(skb->dst == NULL))
		*err = -1;
	else
#ifdef CONFIG_NET_CLS_ROUTE
		dst->value = skb->dst->tclassid;
#else
		dst->value = 0;
#endif
}

META_COLLECTOR(int_rtiif)
{
	if (unlikely(skb->dst == NULL))
		*err = -1;
	else
		dst->value = ((struct rtable*) skb->dst)->fl.iif;
}

/**************************************************************************
 * Socket Attributes
 **************************************************************************/

#define SKIP_NONLOCAL(skb)			\
	if (unlikely(skb->sk == NULL)) {	\
		*err = -1;			\
		return;				\
	}

META_COLLECTOR(int_sk_family)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_family;
}

META_COLLECTOR(int_sk_state)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_state;
}

META_COLLECTOR(int_sk_reuse)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_reuse;
}

META_COLLECTOR(int_sk_bound_if)
{
	SKIP_NONLOCAL(skb);
	/* No error if bound_dev_if is 0, legal userspace check */
	dst->value = skb->sk->sk_bound_dev_if;
}

META_COLLECTOR(var_sk_bound_if)
{
	SKIP_NONLOCAL(skb);

	 if (skb->sk->sk_bound_dev_if == 0) {
		dst->value = (unsigned long) "any";
		dst->len = 3;
	 } else  {
		struct net_device *dev;

		dev = dev_get_by_index(skb->sk->sk_bound_dev_if);
		*err = var_dev(dev, dst);
		if (dev)
			dev_put(dev);
	 }
}

META_COLLECTOR(int_sk_refcnt)
{
	SKIP_NONLOCAL(skb);
	dst->value = atomic_read(&skb->sk->sk_refcnt);
}

META_COLLECTOR(int_sk_rcvbuf)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_rcvbuf;
}

META_COLLECTOR(int_sk_shutdown)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_shutdown;
}

META_COLLECTOR(int_sk_proto)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_protocol;
}

META_COLLECTOR(int_sk_type)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_type;
}

META_COLLECTOR(int_sk_rmem_alloc)
{
	SKIP_NONLOCAL(skb);
	dst->value = sk_rmem_alloc_get(skb->sk);
}

META_COLLECTOR(int_sk_wmem_alloc)
{
	SKIP_NONLOCAL(skb);
	dst->value = sk_wmem_alloc_get(skb->sk);
}

META_COLLECTOR(int_sk_omem_alloc)
{
	SKIP_NONLOCAL(skb);
	dst->value = atomic_read(&skb->sk->sk_omem_alloc);
}

META_COLLECTOR(int_sk_rcv_qlen)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_receive_queue.qlen;
}

META_COLLECTOR(int_sk_snd_qlen)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_write_queue.qlen;
}

META_COLLECTOR(int_sk_wmem_queued)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_wmem_queued;
}

META_COLLECTOR(int_sk_fwd_alloc)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_forward_alloc;
}

META_COLLECTOR(int_sk_sndbuf)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_sndbuf;
}

META_COLLECTOR(int_sk_alloc)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_allocation;
}

META_COLLECTOR(int_sk_route_caps)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_route_caps;
}

META_COLLECTOR(int_sk_hash)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_hash;
}

META_COLLECTOR(int_sk_lingertime)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_lingertime / HZ;
}

META_COLLECTOR(int_sk_err_qlen)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_error_queue.qlen;
}

META_COLLECTOR(int_sk_ack_bl)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_ack_backlog;
}

META_COLLECTOR(int_sk_max_ack_bl)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_max_ack_backlog;
}

META_COLLECTOR(int_sk_prio)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_priority;
}

META_COLLECTOR(int_sk_rcvlowat)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_rcvlowat;
}

META_COLLECTOR(int_sk_rcvtimeo)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_rcvtimeo / HZ;
}

META_COLLECTOR(int_sk_sndtimeo)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_sndtimeo / HZ;
}

META_COLLECTOR(int_sk_sendmsg_off)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_sndmsg_off;
}

META_COLLECTOR(int_sk_write_pend)
{
	SKIP_NONLOCAL(skb);
	dst->value = skb->sk->sk_write_pending;
}

/**************************************************************************
 * Meta value collectors assignment table
 **************************************************************************/

struct meta_ops
{
	void		(*get)(struct sk_buff *, struct tcf_pkt_info *,
			       struct meta_value *, struct meta_obj *, int *);
};

#define META_ID(name) TCF_META_ID_##name
#define META_FUNC(name) { .get = meta_##name }

/* Meta value operations table listing all meta value collectors and
 * assigns them to a type and meta id. */
static struct meta_ops __meta_ops[TCF_META_TYPE_MAX+1][TCF_META_ID_MAX+1] = {
	[TCF_META_TYPE_VAR] = {
		[META_ID(DEV)]			= META_FUNC(var_dev),
		[META_ID(SK_BOUND_IF)] 		= META_FUNC(var_sk_bound_if),
	},
	[TCF_META_TYPE_INT] = {
		[META_ID(RANDOM)]		= META_FUNC(int_random),
		[META_ID(LOADAVG_0)]		= META_FUNC(int_loadavg_0),
		[META_ID(LOADAVG_1)]		= META_FUNC(int_loadavg_1),
		[META_ID(LOADAVG_2)]		= META_FUNC(int_loadavg_2),
		[META_ID(DEV)]			= META_FUNC(int_dev),
		[META_ID(PRIORITY)]		= META_FUNC(int_priority),
		[META_ID(PROTOCOL)]		= META_FUNC(int_protocol),
		[META_ID(PKTTYPE)]		= META_FUNC(int_pkttype),
		[META_ID(PKTLEN)]		= META_FUNC(int_pktlen),
		[META_ID(DATALEN)]		= META_FUNC(int_datalen),
		[META_ID(MACLEN)]		= META_FUNC(int_maclen),
		[META_ID(NFMARK)]		= META_FUNC(int_mark),
		[META_ID(TCINDEX)]		= META_FUNC(int_tcindex),
		[META_ID(RTCLASSID)]		= META_FUNC(int_rtclassid),
		[META_ID(RTIIF)]		= META_FUNC(int_rtiif),
		[META_ID(SK_FAMILY)]		= META_FUNC(int_sk_family),
		[META_ID(SK_STATE)]		= META_FUNC(int_sk_state),
		[META_ID(SK_REUSE)]		= META_FUNC(int_sk_reuse),
		[META_ID(SK_BOUND_IF)]		= META_FUNC(int_sk_bound_if),
		[META_ID(SK_REFCNT)]		= META_FUNC(int_sk_refcnt),
		[META_ID(SK_RCVBUF)]		= META_FUNC(int_sk_rcvbuf),
		[META_ID(SK_SNDBUF)]		= META_FUNC(int_sk_sndbuf),
		[META_ID(SK_SHUTDOWN)]		= META_FUNC(int_sk_shutdown),
		[META_ID(SK_PROTO)]		= META_FUNC(int_sk_proto),
		[META_ID(SK_TYPE)]		= META_FUNC(int_sk_type),
		[META_ID(SK_RMEM_ALLOC)]	= META_FUNC(int_sk_rmem_alloc),
		[META_ID(SK_WMEM_ALLOC)]	= META_FUNC(int_sk_wmem_alloc),
		[META_ID(SK_OMEM_ALLOC)]	= META_FUNC(int_sk_omem_alloc),
		[META_ID(SK_WMEM_QUEUED)]	= META_FUNC(int_sk_wmem_queued),
		[META_ID(SK_RCV_QLEN)]		= META_FUNC(int_sk_rcv_qlen),
		[META_ID(SK_SND_QLEN)]		= META_FUNC(int_sk_snd_qlen),
		[META_ID(SK_ERR_QLEN)]		= META_FUNC(int_sk_err_qlen),
		[META_ID(SK_FORWARD_ALLOCS)]	= META_FUNC(int_sk_fwd_alloc),
		[META_ID(SK_ALLOCS)]		= META_FUNC(int_sk_alloc),
		[META_ID(SK_ROUTE_CAPS)]	= META_FUNC(int_sk_route_caps),
		[META_ID(SK_HASH)]		= META_FUNC(int_sk_hash),
		[META_ID(SK_LINGERTIME)]	= META_FUNC(int_sk_lingertime),
		[META_ID(SK_ACK_BACKLOG)]	= META_FUNC(int_sk_ack_bl),
		[META_ID(SK_MAX_ACK_BACKLOG)]	= META_FUNC(int_sk_max_ack_bl),
		[META_ID(SK_PRIO)]		= META_FUNC(int_sk_prio),
		[META_ID(SK_RCVLOWAT)]		= META_FUNC(int_sk_rcvlowat),
		[META_ID(SK_RCVTIMEO)]		= META_FUNC(int_sk_rcvtimeo),
		[META_ID(SK_SNDTIMEO)]		= META_FUNC(int_sk_sndtimeo),
		[META_ID(SK_SENDMSG_OFF)]	= META_FUNC(int_sk_sendmsg_off),
		[META_ID(SK_WRITE_PENDING)]	= META_FUNC(int_sk_write_pend),
	}
};

static inline struct meta_ops * meta_ops(struct meta_value *val)
{
	return &__meta_ops[meta_type(val)][meta_id(val)];
}

/**************************************************************************
 * Type specific operations for TCF_META_TYPE_VAR
 **************************************************************************/

static int meta_var_compare(struct meta_obj *a, struct meta_obj *b)
{
	int r = a->len - b->len;

	if (r == 0)
		r = memcmp((void *) a->value, (void *) b->value, a->len);

	return r;
}

static int meta_var_change(struct meta_value *dst, struct rtattr *rta)
{
	int len = RTA_PAYLOAD(rta);

	dst->val = (unsigned long)kmemdup(RTA_DATA(rta), len, GFP_KERNEL);
	if (dst->val == 0UL)
		return -ENOMEM;
	dst->len = len;
	return 0;
}

static void meta_var_destroy(struct meta_value *v)
{
	kfree((void *) v->val);
}

static void meta_var_apply_extras(struct meta_value *v,
				  struct meta_obj *dst)
{
	int shift = v->hdr.shift;

	if (shift && shift < dst->len)
		dst->len -= shift;
}

static int meta_var_dump(struct sk_buff *skb, struct meta_value *v, int tlv)
{
	if (v->val && v->len)
		RTA_PUT(skb, tlv, v->len, (void *) v->val);
	return 0;

rtattr_failure:
	return -1;
}

/**************************************************************************
 * Type specific operations for TCF_META_TYPE_INT
 **************************************************************************/

static int meta_int_compare(struct meta_obj *a, struct meta_obj *b)
{
	/* Let gcc optimize it, the unlikely is not really based on
	 * some numbers but jump free code for mismatches seems
	 * more logical. */
	if (unlikely(a->value == b->value))
		return 0;
	else if (a->value < b->value)
		return -1;
	else
		return 1;
}

static int meta_int_change(struct meta_value *dst, struct rtattr *rta)
{
	if (RTA_PAYLOAD(rta) >= sizeof(unsigned long)) {
		dst->val = *(unsigned long *) RTA_DATA(rta);
		dst->len = sizeof(unsigned long);
	} else if (RTA_PAYLOAD(rta) == sizeof(u32)) {
		dst->val = *(u32 *) RTA_DATA(rta);
		dst->len = sizeof(u32);
	} else
		return -EINVAL;

	return 0;
}

static void meta_int_apply_extras(struct meta_value *v,
				  struct meta_obj *dst)
{
	if (v->hdr.shift)
		dst->value >>= v->hdr.shift;

	if (v->val)
		dst->value &= v->val;
}

static int meta_int_dump(struct sk_buff *skb, struct meta_value *v, int tlv)
{
	if (v->len == sizeof(unsigned long))
		RTA_PUT(skb, tlv, sizeof(unsigned long), &v->val);
	else if (v->len == sizeof(u32)) {
		u32 d = v->val;
		RTA_PUT(skb, tlv, sizeof(d), &d);
	}

	return 0;

rtattr_failure:
	return -1;
}

/**************************************************************************
 * Type specific operations table
 **************************************************************************/

struct meta_type_ops
{
	void	(*destroy)(struct meta_value *);
	int	(*compare)(struct meta_obj *, struct meta_obj *);
	int	(*change)(struct meta_value *, struct rtattr *);
	void	(*apply_extras)(struct meta_value *, struct meta_obj *);
	int	(*dump)(struct sk_buff *, struct meta_value *, int);
};

static struct meta_type_ops __meta_type_ops[TCF_META_TYPE_MAX+1] = {
	[TCF_META_TYPE_VAR] = {
		.destroy = meta_var_destroy,
		.compare = meta_var_compare,
		.change = meta_var_change,
		.apply_extras = meta_var_apply_extras,
		.dump = meta_var_dump
	},
	[TCF_META_TYPE_INT] = {
		.compare = meta_int_compare,
		.change = meta_int_change,
		.apply_extras = meta_int_apply_extras,
		.dump = meta_int_dump
	}
};

static inline struct meta_type_ops * meta_type_ops(struct meta_value *v)
{
	return &__meta_type_ops[meta_type(v)];
}

/**************************************************************************
 * Core
 **************************************************************************/

static inline int meta_get(struct sk_buff *skb, struct tcf_pkt_info *info,
			   struct meta_value *v, struct meta_obj *dst)
{
	int err = 0;

	if (meta_id(v) == TCF_META_ID_VALUE) {
		dst->value = v->val;
		dst->len = v->len;
		return 0;
	}

	meta_ops(v)->get(skb, info, v, dst, &err);
	if (err < 0)
		return err;

	if (meta_type_ops(v)->apply_extras)
	    meta_type_ops(v)->apply_extras(v, dst);

	return 0;
}

static int em_meta_match(struct sk_buff *skb, struct tcf_ematch *m,
			 struct tcf_pkt_info *info)
{
	int r;
	struct meta_match *meta = (struct meta_match *) m->data;
	struct meta_obj l_value, r_value;

	if (meta_get(skb, info, &meta->lvalue, &l_value) < 0 ||
	    meta_get(skb, info, &meta->rvalue, &r_value) < 0)
		return 0;

	r = meta_type_ops(&meta->lvalue)->compare(&l_value, &r_value);

	switch (meta->lvalue.hdr.op) {
		case TCF_EM_OPND_EQ:
			return !r;
		case TCF_EM_OPND_LT:
			return r < 0;
		case TCF_EM_OPND_GT:
			return r > 0;
	}

	return 0;
}

static inline void meta_delete(struct meta_match *meta)
{
	struct meta_type_ops *ops = meta_type_ops(&meta->lvalue);

	if (ops && ops->destroy) {
		ops->destroy(&meta->lvalue);
		ops->destroy(&meta->rvalue);
	}

	kfree(meta);
}

static inline int meta_change_data(struct meta_value *dst, struct rtattr *rta)
{
	if (rta) {
		if (RTA_PAYLOAD(rta) == 0)
			return -EINVAL;

		return meta_type_ops(dst)->change(dst, rta);
	}

	return 0;
}

static inline int meta_is_supported(struct meta_value *val)
{
	return (!meta_id(val) || meta_ops(val)->get);
}

static int em_meta_change(struct tcf_proto *tp, void *data, int len,
			  struct tcf_ematch *m)
{
	int err = -EINVAL;
	struct rtattr *tb[TCA_EM_META_MAX];
	struct tcf_meta_hdr *hdr;
	struct meta_match *meta = NULL;

	if (rtattr_parse(tb, TCA_EM_META_MAX, data, len) < 0)
		goto errout;

	if (tb[TCA_EM_META_HDR-1] == NULL ||
	    RTA_PAYLOAD(tb[TCA_EM_META_HDR-1]) < sizeof(*hdr))
		goto errout;
	hdr = RTA_DATA(tb[TCA_EM_META_HDR-1]);

	if (TCF_META_TYPE(hdr->left.kind) != TCF_META_TYPE(hdr->right.kind) ||
	    TCF_META_TYPE(hdr->left.kind) > TCF_META_TYPE_MAX ||
	    TCF_META_ID(hdr->left.kind) > TCF_META_ID_MAX ||
	    TCF_META_ID(hdr->right.kind) > TCF_META_ID_MAX)
		goto errout;

	meta = kzalloc(sizeof(*meta), GFP_KERNEL);
	if (meta == NULL)
		goto errout;

	memcpy(&meta->lvalue.hdr, &hdr->left, sizeof(hdr->left));
	memcpy(&meta->rvalue.hdr, &hdr->right, sizeof(hdr->right));

	if (!meta_is_supported(&meta->lvalue) ||
	    !meta_is_supported(&meta->rvalue)) {
		err = -EOPNOTSUPP;
		goto errout;
	}

	if (meta_change_data(&meta->lvalue, tb[TCA_EM_META_LVALUE-1]) < 0 ||
	    meta_change_data(&meta->rvalue, tb[TCA_EM_META_RVALUE-1]) < 0)
		goto errout;

	m->datalen = sizeof(*meta);
	m->data = (unsigned long) meta;

	err = 0;
errout:
	if (err && meta)
		meta_delete(meta);
	return err;
}

static void em_meta_destroy(struct tcf_proto *tp, struct tcf_ematch *m)
{
	if (m)
		meta_delete((struct meta_match *) m->data);
}

static int em_meta_dump(struct sk_buff *skb, struct tcf_ematch *em)
{
	struct meta_match *meta = (struct meta_match *) em->data;
	struct tcf_meta_hdr hdr;
	struct meta_type_ops *ops;

	memset(&hdr, 0, sizeof(hdr));
	memcpy(&hdr.left, &meta->lvalue.hdr, sizeof(hdr.left));
	memcpy(&hdr.right, &meta->rvalue.hdr, sizeof(hdr.right));

	RTA_PUT(skb, TCA_EM_META_HDR, sizeof(hdr), &hdr);

	ops = meta_type_ops(&meta->lvalue);
	if (ops->dump(skb, &meta->lvalue, TCA_EM_META_LVALUE) < 0 ||
	    ops->dump(skb, &meta->rvalue, TCA_EM_META_RVALUE) < 0)
		goto rtattr_failure;

	return 0;

rtattr_failure:
	return -1;
}

static struct tcf_ematch_ops em_meta_ops = {
	.kind	  = TCF_EM_META,
	.change	  = em_meta_change,
	.match	  = em_meta_match,
	.destroy  = em_meta_destroy,
	.dump	  = em_meta_dump,
	.owner	  = THIS_MODULE,
	.link	  = LIST_HEAD_INIT(em_meta_ops.link)
};

static int __init init_em_meta(void)
{
	return tcf_em_register(&em_meta_ops);
}

static void __exit exit_em_meta(void)
{
	tcf_em_unregister(&em_meta_ops);
}

MODULE_LICENSE("GPL");

module_init(init_em_meta);
module_exit(exit_em_meta);
