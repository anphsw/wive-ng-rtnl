/* Connection state tracking for netfilter.  This is separated from,
   but required by, the NAT layer; it can also be used by an iptables
   extension. */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2006 Netfilter Core Team <coreteam@netfilter.org>
 * (C) 2003,2004 USAGI/WIDE Project <http://www.linux-ipv6.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/stddef.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/sfhash.h>
#include <linux/err.h>
#include <linux/percpu.h>
#include <linux/moduleparam.h>
#include <linux/notifier.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/socket.h>
#include <linux/mm.h>
#include <linux/ip.h>
#include <linux/in.h>

#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE)
#include "../nat/hw_nat/ra_nat.h"
#ifndef CONFIG_RA_NAT_NONE
extern int (*ra_sw_nat_hook_rx)(struct sk_buff *skb);
#endif
#else
static int (*ra_sw_nat_hook_rx)(struct sk_buff * skb) = NULL;
#endif

#if defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR) || defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR_MODULE)
#include <linux/tcp.h>
unsigned int web_str_loaded __read_mostly = 0;
EXPORT_SYMBOL_GPL(web_str_loaded);
#endif

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_l3proto.h>
#include <net/netfilter/nf_conntrack_l4proto.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_core.h>

#define NF_CONNTRACK_VERSION	"0.5.0"

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

rwlock_t nf_conntrack_lock;
EXPORT_SYMBOL_GPL(nf_conntrack_lock);

static void death_by_timeout(unsigned long ul_conntrack);
/* nf_conntrack_standalone needs this */
atomic_t nf_conntrack_count = ATOMIC_INIT(0);
EXPORT_SYMBOL_GPL(nf_conntrack_count);

void (*nf_conntrack_destroyed)(struct nf_conn *conntrack);
EXPORT_SYMBOL_GPL(nf_conntrack_destroyed);

unsigned int nf_conntrack_htable_size __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_htable_size);

unsigned int nf_conntrack_max __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_max);

struct hlist_head *nf_conntrack_hash __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_hash);

struct nf_conn nf_conntrack_untracked;
EXPORT_SYMBOL_GPL(nf_conntrack_untracked);

unsigned int nf_ct_log_invalid __read_mostly;
HLIST_HEAD(unconfirmed);

#ifdef CONFIG_NAT_CONE
unsigned int nf_conntrack_nat_mode __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_nat_mode);
extern char wan_name[IFNAMSIZ];
#if defined (CONFIG_PPP) || defined (CONFIG_PPP_MODULE)
extern char wan_ppp[IFNAMSIZ];
#endif
#endif

#ifdef CONFIG_BCM_NAT
unsigned int nf_conntrack_fastnat __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_fastnat);
unsigned int nf_conntrack_fastroute __read_mostly;
EXPORT_SYMBOL_GPL(nf_conntrack_fastroute);
#endif

#ifdef CONFIG_NF_FLUSH_CONNTRACK
unsigned int nf_conntrack_table_flush __read_mostly;
#endif

static int nf_conntrack_vmalloc __read_mostly;

DEFINE_PER_CPU(struct ip_conntrack_stat, nf_conntrack_stat);
EXPORT_PER_CPU_SYMBOL(nf_conntrack_stat);

/*
 * This scheme offers various size of "struct nf_conn" dependent on
 * features(helper, nat, ...)
 */

#define NF_CT_FEATURES_NAMELEN	256
static struct {
	/* name of slab cache. printed in /proc/slabinfo */
	char *name;

	/* size of slab cache */
	size_t size;

	/* slab cache pointer */
	struct kmem_cache *cachep;

	/* allocated slab cache + modules which uses this slab cache */
	int use;

} nf_ct_cache[NF_CT_F_NUM];

/* protect members of nf_ct_cache except of "use" */
DEFINE_RWLOCK(nf_ct_cache_lock);

/* This avoids calling kmem_cache_create() with same name simultaneously */
static DEFINE_MUTEX(nf_ct_cache_mutex);

static unsigned int nf_conntrack_hash_rnd __read_mostly;

#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE)
static inline unsigned int is_local_svc(u_int8_t protonm)
{
	/* Local gre/esp/ah/ip-ip/ipv6_in_ipv4/icmp proto must be skip from hardware offload
	    and mark as interested by ALG  for correct tracking this */
	switch (protonm) {
	    case IPPROTO_IPIP:
#if defined(CONFIG_IPV6) && defined(RA_HW_NAT_IPV6)
#ifndef CONFIG_HNAT_V2
	    case IPPROTO_IPV6:
#endif
#endif
	    case IPPROTO_ICMP:
	    case IPPROTO_GRE:
	    case IPPROTO_ESP:
	    case IPPROTO_AH:
		return 1;
	    default:
		return 0;
	}

    return 0;
};
#endif

static u_int32_t __hash_conntrack(const struct nf_conntrack_tuple *tuple,
				  unsigned int size, unsigned int rnd)
{
	unsigned int a, b;

#ifdef CONFIG_NAT_CONE
	if (nf_conntrack_nat_mode == NAT_MODE_FCONE) {
	    a = jhash2(tuple->dst.u3.all, ARRAY_SIZE(tuple->dst.u3.all), tuple->dst.u.all); // dst ip, dst port
	    b = jhash2(tuple->dst.u3.all, ARRAY_SIZE(tuple->dst.u3.all), tuple->dst.protonum); //dst ip, & dst ip protocol
	} else if (nf_conntrack_nat_mode == NAT_MODE_RCONE) {
	    a = jhash2(tuple->src.u3.all, ARRAY_SIZE(tuple->src.u3.all), (tuple->src.l3num << 16) | tuple->dst.protonum);
	    b = jhash2(tuple->dst.u3.all, ARRAY_SIZE(tuple->dst.u3.all), (tuple->dst.u.all << 16) | tuple->dst.protonum);
	} else {
#endif
	    /* Original linux logic */
	    a = jhash2(tuple->src.u3.all, ARRAY_SIZE(tuple->src.u3.all), ((tuple->src.l3num) << 16) | tuple->dst.protonum);
	    b = jhash2(tuple->dst.u3.all, ARRAY_SIZE(tuple->dst.u3.all), (tuple->src.u.all << 16) | tuple->dst.u.all);
#ifdef CONFIG_NAT_CONE
	}
#endif
	return ((u64)HASH_2WORDS(a, b, rnd) * size) >> 32;
}

static inline u_int32_t hash_conntrack(const struct nf_conntrack_tuple *tuple)
{
	return __hash_conntrack(tuple, nf_conntrack_htable_size,
				nf_conntrack_hash_rnd);
}

int nf_conntrack_register_cache(u_int32_t features, const char *name,
				size_t size)
{
	int ret = 0;
	char *cache_name;
	struct kmem_cache *cachep;

	DEBUGP("nf_conntrack_register_cache: features=0x%x, name=%s, size=%d\n",
	       features, name, size);

	if (features < NF_CT_F_BASIC || features >= NF_CT_F_NUM) {
		DEBUGP("nf_conntrack_register_cache: invalid features.: 0x%x\n",
			features);
		return -EINVAL;
	}

	mutex_lock(&nf_ct_cache_mutex);

	write_lock_bh(&nf_ct_cache_lock);
	/* e.g: multiple helpers are loaded */
	if (nf_ct_cache[features].use > 0) {
		DEBUGP("nf_conntrack_register_cache: already resisterd.\n");
		if ((!strncmp(nf_ct_cache[features].name, name,
			      NF_CT_FEATURES_NAMELEN))
		    && nf_ct_cache[features].size == size) {
			DEBUGP("nf_conntrack_register_cache: reusing.\n");	// for different features
			nf_ct_cache[features].use++;
			ret = 0;
		} else
			ret = -EBUSY;

		write_unlock_bh(&nf_ct_cache_lock);
		mutex_unlock(&nf_ct_cache_mutex);
		return ret;
	}
	write_unlock_bh(&nf_ct_cache_lock);

	/*
	 * The memory space for name of slab cache must be alive until
	 * cache is destroyed.
	 */
	cache_name = kmalloc(sizeof(char)*NF_CT_FEATURES_NAMELEN, GFP_ATOMIC);
	if (cache_name == NULL) {
		DEBUGP("nf_conntrack_register_cache: can't alloc cache_name\n");
		ret = -ENOMEM;
		goto out_up_mutex;
	}

	if (strlcpy(cache_name, name, NF_CT_FEATURES_NAMELEN)
						>= NF_CT_FEATURES_NAMELEN) {
		printk("nf_conntrack_register_cache: name too long\n");
		ret = -EINVAL;
		goto out_free_name;
	}

	cachep = kmem_cache_create(cache_name, size, 0, 0,
				   NULL, NULL);
	if (!cachep) {
		printk("nf_conntrack_register_cache: Can't create slab cache "
		       "for the features = 0x%x\n", features);
		ret = -ENOMEM;
		goto out_free_name;
	}

	write_lock_bh(&nf_ct_cache_lock);
	nf_ct_cache[features].use = 1;
	nf_ct_cache[features].size = size;
	nf_ct_cache[features].cachep = cachep;
	nf_ct_cache[features].name = cache_name;
	write_unlock_bh(&nf_ct_cache_lock);

	goto out_up_mutex;

out_free_name:
	kfree(cache_name);
out_up_mutex:
	mutex_unlock(&nf_ct_cache_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(nf_conntrack_register_cache);

/* FIXME: In the current, only nf_conntrack_cleanup() can call this function. */
void nf_conntrack_unregister_cache(u_int32_t features)
{
	struct kmem_cache *cachep;
	char *name;

	/*
	 * This assures that kmem_cache_create() isn't called before destroying
	 * slab cache.
	 */
	DEBUGP("nf_conntrack_unregister_cache: 0x%04x\n", features);
	mutex_lock(&nf_ct_cache_mutex);

	write_lock_bh(&nf_ct_cache_lock);
	if (--nf_ct_cache[features].use > 0) {
		write_unlock_bh(&nf_ct_cache_lock);
		mutex_unlock(&nf_ct_cache_mutex);
		return;
	}
	cachep = nf_ct_cache[features].cachep;
	name = nf_ct_cache[features].name;
	nf_ct_cache[features].cachep = NULL;
	nf_ct_cache[features].name = NULL;
	nf_ct_cache[features].size = 0;
	write_unlock_bh(&nf_ct_cache_lock);

	synchronize_net();

	kmem_cache_destroy(cachep);
	kfree(name);

	mutex_unlock(&nf_ct_cache_mutex);
}
EXPORT_SYMBOL_GPL(nf_conntrack_unregister_cache);

int FASTPATHNET nf_ct_get_tuple(const struct sk_buff *skb,
		unsigned int nhoff,
		unsigned int dataoff,
		u_int16_t l3num,
		u_int8_t protonum,
		struct nf_conntrack_tuple *tuple,
		const struct nf_conntrack_l3proto *l3proto,
		const struct nf_conntrack_l4proto *l4proto)
{
	memset(tuple, 0, sizeof(*tuple));

	tuple->src.l3num = l3num;
	if (l3proto->pkt_to_tuple(skb, nhoff, tuple) == 0)
		return 0;

	tuple->dst.protonum = protonum;
	tuple->dst.dir = IP_CT_DIR_ORIGINAL;

	return l4proto->pkt_to_tuple(skb, dataoff, tuple);
}
EXPORT_SYMBOL_GPL(nf_ct_get_tuple);

int FASTPATHNET nf_ct_invert_tuple(struct nf_conntrack_tuple *inverse,
		   const struct nf_conntrack_tuple *orig,
		   const struct nf_conntrack_l3proto *l3proto,
		   const struct nf_conntrack_l4proto *l4proto)
{
	memset(inverse, 0, sizeof(*inverse));

	inverse->src.l3num = orig->src.l3num;
	if (l3proto->invert_tuple(inverse, orig) == 0)
		return 0;

	inverse->dst.dir = !orig->dst.dir;

	inverse->dst.protonum = orig->dst.protonum;
	return l4proto->invert_tuple(inverse, orig);
}
EXPORT_SYMBOL_GPL(nf_ct_invert_tuple);

static void
clean_from_lists(struct nf_conn *ct)
{
	DEBUGP("clean_from_lists(%p)\n", ct);
	hlist_del(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnode);
	hlist_del(&ct->tuplehash[IP_CT_DIR_REPLY].hnode);

	/* Destroy all pending expectations */
	nf_ct_remove_expectations(ct);
}

static void
destroy_conntrack(struct nf_conntrack *nfct)
{
	struct nf_conn *ct = (struct nf_conn *)nfct;
	struct nf_conntrack_l4proto *l4proto;
	typeof(nf_conntrack_destroyed) destroyed;

	DEBUGP("destroy_conntrack(%p)\n", ct);
	NF_CT_ASSERT(atomic_read(&nfct->use) == 0);
	NF_CT_ASSERT(!timer_pending(&ct->timeout));

	nf_conntrack_event(IPCT_DESTROY, ct);
	set_bit(IPS_DYING_BIT, &ct->status);

	/* To make sure we don't get any weird locking issues here:
	 * destroy_conntrack() MUST NOT be called with a write lock
	 * to nf_conntrack_lock!!! -HW */
	rcu_read_lock();
	l4proto = __nf_ct_l4proto_find(ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.l3num,
				       ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.protonum);
	if (l4proto && l4proto->destroy)
		l4proto->destroy(ct);

	destroyed = rcu_dereference(nf_conntrack_destroyed);
	if (destroyed)
		destroyed(ct);

	rcu_read_unlock();

	write_lock_bh(&nf_conntrack_lock);
	/* Expectations will have been removed in clean_from_lists,
	 * except TFTP can create an expectation on the first packet,
	 * before connection is in the list, so we need to clean here,
	 * too. */
	nf_ct_remove_expectations(ct);

	#if defined(CONFIG_NETFILTER_XT_MATCH_LAYER7) || defined(CONFIG_NETFILTER_XT_MATCH_LAYER7_MODULE)
	if(ct->layer7.app_proto)
		kfree(ct->layer7.app_proto);
	if(ct->layer7.app_data)
	kfree(ct->layer7.app_data);
	#endif


	/* We overload first tuple to link into unconfirmed list. */
	if (!nf_ct_is_confirmed(ct)) {
		BUG_ON(hlist_unhashed(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnode));
		hlist_del(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnode);
	}

	NF_CT_STAT_INC(delete);
	write_unlock_bh(&nf_conntrack_lock);

	if (ct->master)
		nf_ct_put(ct->master);

	DEBUGP("destroy_conntrack: returning ct=%p to slab\n", ct);
	nf_conntrack_free(ct);
}

static void death_by_timeout(unsigned long ul_conntrack)
{
	struct nf_conn *ct = (void *)ul_conntrack;
	struct nf_conn_help *help = nfct_help(ct);

	if (help && help->helper && help->helper->destroy)
		help->helper->destroy(ct);

	write_lock_bh(&nf_conntrack_lock);
	/* Inside lock so preempt is disabled on module removal path.
	 * Otherwise we can get spurious warnings. */
	NF_CT_STAT_INC(delete_list);
	clean_from_lists(ct);
	write_unlock_bh(&nf_conntrack_lock);
	nf_ct_put(ct);
}

struct FASTPATHNET nf_conntrack_tuple_hash *
__nf_conntrack_find(const struct nf_conntrack_tuple *tuple,
		    const struct nf_conn *ignored_conntrack)
{
	struct nf_conntrack_tuple_hash *h;
	struct hlist_node *n;
	unsigned int hash = hash_conntrack(tuple);

#ifdef CONFIG_NF_FLUSH_CONNTRACK
    	if (unlikely(nf_conntrack_table_flush) && (atomic_read(&nf_conntrack_count))) {
	    DEBUGP("nf_conntrack_find: clear connection track table\n");
	    nf_conntrack_flush();
	    nf_conntrack_table_flush=0;
    	}
#endif
	hlist_for_each_entry(h, n, &nf_conntrack_hash[hash], hnode) {
		if (nf_ct_tuplehash_to_ctrack(h) != ignored_conntrack &&
		    nf_ct_tuple_equal(tuple, &h->tuple)) {
			NF_CT_STAT_INC(found);
			return h;
		}
		NF_CT_STAT_INC(searched);
	}

	return NULL;
}
EXPORT_SYMBOL_GPL(__nf_conntrack_find);

#ifdef CONFIG_NAT_CONE
static int nf_ct_cone_tuple_equal(const struct nf_conntrack_tuple *t1,
                                    const struct nf_conntrack_tuple *t2)
{
	if (nf_conntrack_nat_mode == NAT_MODE_FCONE)    /* Full Cone */
    	    return nf_ct_tuple_dst_equal(t1, t2);
	else if (nf_conntrack_nat_mode == NAT_MODE_RCONE)    /* Restricted Cone */
	    return (nf_ct_tuple_dst_equal(t1, t2) &&
	        t1->src.u3.all[0] == t2->src.u3.all[0] &&
                t1->src.u3.all[1] == t2->src.u3.all[1] &&
		t1->src.u3.all[2] == t2->src.u3.all[2] &&
                t1->src.u3.all[3] == t2->src.u3.all[3] &&
		t1->src.l3num == t2->src.l3num &&
		t1->dst.protonum == t2->dst.protonum);
	else return 0;
}

static struct nf_conntrack_tuple_hash *
__nf_cone_conntrack_find(const struct nf_conntrack_tuple *tuple,
        const struct nf_conn *ignored_conntrack)
{
    struct nf_conntrack_tuple_hash *h;
    struct hlist_node *n;
    unsigned int hash = hash_conntrack(tuple);

    hlist_for_each_entry(h, n, &nf_conntrack_hash[hash], hnode) {
        if (nf_ct_tuplehash_to_ctrack(h) != ignored_conntrack &&
                nf_ct_cone_tuple_equal(tuple, &h->tuple)) {
            NF_CT_STAT_INC(found);
            return h;
        }
        NF_CT_STAT_INC(searched);
    }

    return NULL;
}

/* Find a connection corresponding to a tuple. */
static struct nf_conntrack_tuple_hash *
nf_cone_conntrack_find_get(const struct nf_conntrack_tuple *tuple,
        const struct nf_conn *ignored_conntrack)
{
    struct nf_conntrack_tuple_hash *h;

    read_lock_bh(&nf_conntrack_lock);
    h = __nf_cone_conntrack_find(tuple, ignored_conntrack);
    if (h)
        atomic_inc(&nf_ct_tuplehash_to_ctrack(h)->ct_general.use);
    read_unlock_bh(&nf_conntrack_lock);

    return h;
}
#endif

/* Find a connection corresponding to a tuple. */
struct FASTPATHNET nf_conntrack_tuple_hash *
nf_conntrack_find_get(const struct nf_conntrack_tuple *tuple)
{
    struct nf_conntrack_tuple_hash *h;

    read_lock_bh(&nf_conntrack_lock);
    h = __nf_conntrack_find(tuple, NULL);
    if (h)
	atomic_inc(&nf_ct_tuplehash_to_ctrack(h)->ct_general.use);
    read_unlock_bh(&nf_conntrack_lock);

    return h;
}
EXPORT_SYMBOL_GPL(nf_conntrack_find_get);

static void __nf_conntrack_hash_insert(struct nf_conn *ct,
				       unsigned int hash,
				       unsigned int repl_hash)
{
	hlist_add_head(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnode,
		       &nf_conntrack_hash[hash]);
	hlist_add_head(&ct->tuplehash[IP_CT_DIR_REPLY].hnode,
		       &nf_conntrack_hash[repl_hash]);
}

void FASTPATHNET nf_conntrack_hash_insert(struct nf_conn *ct)
{
	unsigned int hash, repl_hash;

	hash = hash_conntrack(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
	repl_hash = hash_conntrack(&ct->tuplehash[IP_CT_DIR_REPLY].tuple);

	write_lock_bh(&nf_conntrack_lock);
	__nf_conntrack_hash_insert(ct, hash, repl_hash);
	write_unlock_bh(&nf_conntrack_lock);
}
EXPORT_SYMBOL_GPL(nf_conntrack_hash_insert);

/* Confirm a connection given skb; places it in hash table */
int FASTPATHNET __nf_conntrack_confirm(struct sk_buff **pskb)
{
	unsigned int hash, repl_hash;
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
	struct nf_conn_help *help;
	struct hlist_node *n;
	enum ip_conntrack_info ctinfo;

	ct = nf_ct_get(*pskb, &ctinfo);

	/* ipt_REJECT uses nf_conntrack_attach to attach related
	   ICMP/TCP RST packets in other direction.  Actual packet
	   which created connection will be IP_CT_NEW or for an
	   expected connection, IP_CT_RELATED. */
	if (CTINFO2DIR(ctinfo) != IP_CT_DIR_ORIGINAL)
		return NF_ACCEPT;

	hash = hash_conntrack(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
	repl_hash = hash_conntrack(&ct->tuplehash[IP_CT_DIR_REPLY].tuple);

	/* We're not in hash table, and we refuse to set up related
	   connections for unconfirmed conns.  But packet copies and
	   REJECT will give spurious warnings here. */
	/* NF_CT_ASSERT(atomic_read(&ct->ct_general.use) == 1); */

	/* No external references means noone else could have
	   confirmed us. */
	NF_CT_ASSERT(!nf_ct_is_confirmed(ct));
	DEBUGP("Confirming conntrack %p\n", ct);

	write_lock_bh(&nf_conntrack_lock);

	/* We have to check the DYING flag inside the lock to prevent
	   a race against nf_ct_get_next_corpse() possibly called from
	   user context, else we insert an already 'dead' hash, blocking
	   further use of that particular connection -JM */

	if (unlikely(nf_ct_is_dying(ct))) {
		write_unlock_bh(&nf_conntrack_lock);
		return NF_ACCEPT;
	}

	/* See if there's one in the list already, including reverse:
	   NAT could have grabbed it without realizing, since we're
	   not in the hash.  If there is, we lost race. */
	hlist_for_each_entry(h, n, &nf_conntrack_hash[hash], hnode)
		if (nf_ct_tuple_equal(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple,
				      &h->tuple))
			goto out;
	hlist_for_each_entry(h, n, &nf_conntrack_hash[repl_hash], hnode)
		if (nf_ct_tuple_equal(&ct->tuplehash[IP_CT_DIR_REPLY].tuple,
				      &h->tuple))
			goto out;

	/* Remove from unconfirmed list */
	hlist_del(&ct->tuplehash[IP_CT_DIR_ORIGINAL].hnode);

	__nf_conntrack_hash_insert(ct, hash, repl_hash);
	/* Timer relative to confirmation time, not original
	   setting time, otherwise we'd get timer wrap in
	   weird delay cases. */
	ct->timeout.expires += jiffies;
	add_timer(&ct->timeout);
	atomic_inc(&ct->ct_general.use);
	ct->status |= IPS_CONFIRMED;
	NF_CT_STAT_INC(insert);
	write_unlock_bh(&nf_conntrack_lock);
	help = nfct_help(ct);
	if (help && help->helper)
		nf_conntrack_event_cache(IPCT_HELPER, *pskb);
#ifdef CONFIG_NF_NAT_NEEDED
	if (test_bit(IPS_SRC_NAT_DONE_BIT, &ct->status) ||
	    test_bit(IPS_DST_NAT_DONE_BIT, &ct->status))
		nf_conntrack_event_cache(IPCT_NATINFO, *pskb);
#endif
	nf_conntrack_event_cache(master_ct(ct) ?
				 IPCT_RELATED : IPCT_NEW, *pskb);
	return NF_ACCEPT;

out:
	NF_CT_STAT_INC(insert_failed);
	write_unlock_bh(&nf_conntrack_lock);
	return NF_DROP;
}
EXPORT_SYMBOL_GPL(__nf_conntrack_confirm);

/* Returns true if a connection correspondings to the tuple (required
   for NAT). */
int
nf_conntrack_tuple_taken(const struct nf_conntrack_tuple *tuple,
			 const struct nf_conn *ignored_conntrack)
{
	struct nf_conntrack_tuple_hash *h;

	read_lock_bh(&nf_conntrack_lock);
	h = __nf_conntrack_find(tuple, ignored_conntrack);
	read_unlock_bh(&nf_conntrack_lock);

	return h != NULL;
}
EXPORT_SYMBOL_GPL(nf_conntrack_tuple_taken);

#define NF_CT_EVICTION_RANGE	8

/* There's a small race here where we may free a just-assured
   connection.  Too bad: we're in trouble anyway. */
static noinline int early_drop(unsigned int hash)
{
	/* Use oldest entry, which is roughly LRU */
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct = NULL, *tmp;
	struct hlist_node *n;
	unsigned int i, cnt = 0;
	int dropped = 0;

	read_lock_bh(&nf_conntrack_lock);
	for (i = 0; i < nf_conntrack_htable_size; i++) {
		hlist_for_each_entry(h, n, &nf_conntrack_hash[hash], hnode) {
			tmp = nf_ct_tuplehash_to_ctrack(h);
			if (!test_bit(IPS_ASSURED_BIT, &tmp->status))
				ct = tmp;
			cnt++;
		}
		if (ct || cnt >= NF_CT_EVICTION_RANGE)
			break;
		hash = (hash + 1) % nf_conntrack_htable_size;
	}
	if (ct)
		atomic_inc(&ct->ct_general.use);
	read_unlock_bh(&nf_conntrack_lock);

	if (!ct)
		return dropped;

	if (del_timer(&ct->timeout)) {
		death_by_timeout((unsigned long)ct);
		dropped = 1;
		NF_CT_STAT_INC_ATOMIC(early_drop);
	}
	nf_ct_put(ct);
	return dropped;
}

static struct nf_conn *
__nf_conntrack_alloc(const struct nf_conntrack_tuple *orig,
		     const struct nf_conntrack_tuple *repl,
		     const struct nf_conntrack_l3proto *l3proto,
		     u_int32_t features)
{
	struct nf_conn *conntrack = NULL;
	struct nf_conntrack_helper *helper;

	if (unlikely(!nf_conntrack_hash_rnd)) {
		unsigned int rand;

		/*
		 * Why not initialize nf_conntrack_rnd in a "init()" function ?
		 * Because there isn't enough entropy when system initializing,
		 * and we initialize it as late as possible.
		 */
		do {
			get_random_bytes(&rand, sizeof(rand));
		} while (!rand);
		cmpxchg(&nf_conntrack_hash_rnd, 0, rand);
	}

	/* We don't want any race condition at early drop stage */
	atomic_inc(&nf_conntrack_count);

	if (nf_conntrack_max &&
	    unlikely(atomic_read(&nf_conntrack_count) > nf_conntrack_max)) {
		unsigned int hash = hash_conntrack(orig);
		if (!early_drop(hash)) {
			atomic_dec(&nf_conntrack_count);
			if (net_ratelimit())
				printk(KERN_WARNING
				       "nf_conntrack: table full, dropping"
				       " packet.\n");
			return ERR_PTR(-ENOMEM);
		}
	}

	/*  find features needed by this conntrack. */
	features |= l3proto->get_features(orig);

	/* FIXME: protect helper list per RCU */
	read_lock_bh(&nf_conntrack_lock);
	helper = __nf_ct_helper_find(repl);
	/* NAT might want to assign a helper later */
	if (helper || features & NF_CT_F_NAT)
		features |= NF_CT_F_HELP;
	read_unlock_bh(&nf_conntrack_lock);

	DEBUGP("nf_conntrack_alloc: features=0x%x\n", features);

	read_lock_bh(&nf_ct_cache_lock);

	if (unlikely(!nf_ct_cache[features].use)) {
		DEBUGP("nf_conntrack_alloc: not supported features = 0x%x\n",
			features);
		goto out;
	}

	conntrack = kmem_cache_zalloc(nf_ct_cache[features].cachep, GFP_ATOMIC);
	if (conntrack == NULL) {
		DEBUGP("nf_conntrack_alloc: Can't alloc conntrack from cache\n");
		goto out;
	}

	conntrack->features = features;
	atomic_set(&conntrack->ct_general.use, 1);
	conntrack->tuplehash[IP_CT_DIR_ORIGINAL].tuple = *orig;
	conntrack->tuplehash[IP_CT_DIR_REPLY].tuple = *repl;
	/* Don't set timer yet: wait for confirmation */
	setup_timer(&conntrack->timeout, death_by_timeout,
		    (unsigned long)conntrack);
	read_unlock_bh(&nf_ct_cache_lock);

	return conntrack;
out:
	read_unlock_bh(&nf_ct_cache_lock);
	atomic_dec(&nf_conntrack_count);
	return conntrack;
}

struct nf_conn *nf_conntrack_alloc(const struct nf_conntrack_tuple *orig,
				   const struct nf_conntrack_tuple *repl)
{
	struct nf_conntrack_l3proto *l3proto;
	struct nf_conn *ct;

	rcu_read_lock();
	l3proto = __nf_ct_l3proto_find(orig->src.l3num);
	ct = __nf_conntrack_alloc(orig, repl, l3proto, 0);
	rcu_read_unlock();

	return ct;
}
EXPORT_SYMBOL_GPL(nf_conntrack_alloc);

void nf_conntrack_free(struct nf_conn *conntrack)
{
	u_int32_t features = conntrack->features;
	NF_CT_ASSERT(features >= NF_CT_F_BASIC && features < NF_CT_F_NUM);
	DEBUGP("nf_conntrack_free: features = 0x%x, conntrack=%p\n", features,
	       conntrack);
	kmem_cache_free(nf_ct_cache[features].cachep, conntrack);
	atomic_dec(&nf_conntrack_count);
}
EXPORT_SYMBOL_GPL(nf_conntrack_free);

/* Allocate a new conntrack: we return -ENOMEM if classification
   failed due to stress.  Otherwise it really is unclassifiable. */
static struct nf_conntrack_tuple_hash *
init_conntrack(const struct nf_conntrack_tuple *tuple,
	       struct nf_conntrack_l3proto *l3proto,
	       struct nf_conntrack_l4proto *l4proto,
	       struct sk_buff *skb,
	       unsigned int dataoff)
{
	struct nf_conn *conntrack;
	struct nf_conntrack_tuple repl_tuple;
	struct nf_conntrack_expect *exp;
	u_int32_t features = 0;

	if (!nf_ct_invert_tuple(&repl_tuple, tuple, l3proto, l4proto)) {
		DEBUGP("Can't invert tuple.\n");
		return NULL;
	}

	read_lock_bh(&nf_conntrack_lock);
	exp = __nf_conntrack_expect_find(tuple);
	if (exp && exp->helper)
		features = NF_CT_F_HELP;
	read_unlock_bh(&nf_conntrack_lock);

	conntrack = __nf_conntrack_alloc(tuple, &repl_tuple, l3proto, features);
	if (conntrack == NULL || IS_ERR(conntrack)) {
		DEBUGP("Can't allocate conntrack.\n");
		return (struct nf_conntrack_tuple_hash *)conntrack;
	}

	if (!l4proto->new(conntrack, skb, dataoff)) {
		nf_conntrack_free(conntrack);
		DEBUGP("init conntrack: can't track with proto module\n");
		DEBUGP("l4proto name = %s\n", l4proto->name);
		return NULL;
	}

	write_lock_bh(&nf_conntrack_lock);
	exp = find_expectation(tuple);

	if (exp) {
		DEBUGP("conntrack: expectation arrives ct=%p exp=%p\n",
			conntrack, exp);
		/* Welcome, Mr. Bond.  We've been expecting you... */
		__set_bit(IPS_EXPECTED_BIT, &conntrack->status);
		conntrack->master = exp->master;
		if (exp->helper)
			nfct_help(conntrack)->helper = exp->helper;
#ifdef CONFIG_NF_CONNTRACK_MARK
		conntrack->mark = exp->master->mark;
#endif
#ifdef CONFIG_NF_CONNTRACK_SECMARK
		conntrack->secmark = exp->master->secmark;
#endif
#ifdef CONFIG_BCM_NAT
		conntrack->fastnat = NF_FAST_NAT_DENY;
#endif
		nf_conntrack_get(&conntrack->master->ct_general);
		NF_CT_STAT_INC(expect_new);
	} else {
		struct nf_conn_help *help = nfct_help(conntrack);

		if (help)
			help->helper = __nf_ct_helper_find(&repl_tuple);
		NF_CT_STAT_INC(new);
	}

	/* Overload tuple linked list to put us in unconfirmed list. */
	hlist_add_head(&conntrack->tuplehash[IP_CT_DIR_ORIGINAL].hnode,
		       &unconfirmed);

	write_unlock_bh(&nf_conntrack_lock);

	if (exp) {
		if (exp->expectfn)
			exp->expectfn(conntrack, exp);
		nf_conntrack_expect_put(exp);
	}

	return &conntrack->tuplehash[IP_CT_DIR_ORIGINAL];
}

/* On success, returns conntrack ptr, sets skb->nfct and ctinfo */
static FASTPATHNET struct nf_conn *
resolve_normal_ct(struct sk_buff *skb,
		  unsigned int dataoff,
		  u_int16_t l3num,
		  u_int8_t protonum,
		  struct nf_conntrack_l3proto *l3proto,
		  struct nf_conntrack_l4proto *l4proto,
		  int *set_reply,
		  enum ip_conntrack_info *ctinfo)
{
	struct nf_conntrack_tuple tuple;
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
#ifdef CONFIG_NAT_CONE
	struct iphdr *iph;
#endif

	if (!nf_ct_get_tuple(skb, skb_network_offset(skb),
			     dataoff, l3num, protonum, &tuple, l3proto,
			     l4proto)) {
		DEBUGP("resolve_normal_ct: Can't get tuple\n");
		return NULL;
	}

	/* look for tuple match */
#ifdef CONFIG_NAT_CONE
        /*
         * Based on NAT treatments of UDP in RFC3489:
         *
         * 1)Full Cone: A full cone NAT is one where all requests from the
         * same internal IP address and port are mapped to the same external
         * IP address and port.  Furthermore, any external host can send a
         * packet to the internal host, by sending a packet to the mapped
         * external address.
         *
         * 2)Restricted Cone: A restricted cone NAT is one where all requests
         * from the same internal IP address and port are mapped to the same
         * external IP address and port.  Unlike a full cone NAT, an external
         * host (with IP address X) can send a packet to the internal host
         * only if the internal host had previously sent a packet to IP
         * address X.
         *
         * 3)Port Restricted Cone: A port restricted cone NAT is like a
         * restricted cone NAT, but the restriction includes port numbers.
         * Specifically, an external host can send a packet, with source IP
         * address X and source port P, to the internal host only if the
         * internal host had previously sent a packet to IP address X and
         * port P.
         *
         * 4)Symmetric: A symmetric NAT is one where all requests from the
         * same internal IP address and port, to a specific destination IP
         * address and port, are mapped to the same external IP address and
         * port.  If the same host sends a packet with the same source
         * address and port, but to a different destination, a different
         * mapping is used.  Furthermore, only the external host that
         * receives a packet can send a UDP packet back to the internal host.
         *
         *
	 *
         *
         * Original Linux NAT type is hybrid 'port restricted cone' and
         * 'symmetric'. XBOX certificate recommands NAT type is 'fully cone'
         * or 'restricted cone', so i patch the linux kernel to support
         * this feature
         * Tradition scenario from LAN->WAN:
         *
         *        (LAN)     (WAN)
         * Client------>AP---------> Server
         * -------------> (I)
         *              -------------->(II)
         *              <--------------(III)
         * <------------- (IV)
         *
         *
         * (CASE I/II/IV) Compared Tuple=src_ip/port & dst_ip/port & proto
         * (CASE III)  Compared Tuple:
         *             Fully cone=dst_ip/port & proto
         *             Restricted Cone=dst_ip/port & proto & src_ip
         *
         */
	iph = skb->nh.iph;

	if ((nf_conntrack_nat_mode > 0) && (iph != NULL && iph->protocol == IPPROTO_UDP)) {
#if defined (CONFIG_PPP) || defined (CONFIG_PPP_MODULE)
		if ((skb->dev != NULL) && (strcmp(skb->dev->name, wan_name) == 0 || strcmp(skb->dev->name, wan_ppp) == 0))
#else
		if ((skb->dev != NULL) && (strcmp(skb->dev->name, wan_name) == 0))
#endif
		    /* CASE III To Cone NAT */
		    h = nf_cone_conntrack_find_get(&tuple, NULL);
		else
		    /* CASE I.II.IV To Linux NAT */
		    h = nf_conntrack_find_get(&tuple);
	} else
#endif
	/* CASE I.II.IV To Linux NAT */
	h = nf_conntrack_find_get(&tuple);

	if (!h) {
		h = init_conntrack(&tuple, l3proto, l4proto, skb, dataoff);
		if (!h)
			return NULL;
		if (IS_ERR(h))
			return (void *)h;
	}
	ct = nf_ct_tuplehash_to_ctrack(h);

	/* It exists; we have (non-exclusive) reference. */
	if (NF_CT_DIRECTION(h) == IP_CT_DIR_REPLY) {
		*ctinfo = IP_CT_ESTABLISHED_REPLY;
		/* Please set reply bit if this packet OK */
		*set_reply = 1;
	} else {
		/* Once we've had two way comms, always ESTABLISHED. */
		if (test_bit(IPS_SEEN_REPLY_BIT, &ct->status)) {
			DEBUGP("nf_conntrack_in: normal packet for %p\n", ct);
			*ctinfo = IP_CT_ESTABLISHED;
		} else if (test_bit(IPS_EXPECTED_BIT, &ct->status)) {
			DEBUGP("nf_conntrack_in: related packet for %p\n", ct);
			*ctinfo = IP_CT_RELATED;
		} else {
			DEBUGP("nf_conntrack_in: new packet for %p\n", ct);
			*ctinfo = IP_CT_NEW;
		}
		*set_reply = 0;
	}
	skb->nfct = &ct->ct_general;
	skb->nfctinfo = *ctinfo;
	return ct;
}

unsigned int FASTPATHNET nf_conntrack_in(int pf, unsigned int hooknum, struct sk_buff **pskb)
{
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	struct nf_conntrack_l3proto *l3proto;
	struct nf_conntrack_l4proto *l4proto;
	unsigned int dataoff;
	u_int8_t protonum;
	int set_reply = 0;
	int ret;
#ifdef CONFIG_BCM_NAT
	int pure_route = 0;
	extern int skb_is_ready(struct sk_buff *skb);
	extern int is_pure_routing(struct nf_conn *ct);
	extern int bcm_do_fastroute(struct nf_conn *ct,	struct sk_buff **pskb, unsigned int hooknum, int set_reply);
	extern int bcm_do_fastnat(struct nf_conn *ct, enum ip_conntrack_info ctinfo, struct sk_buff **pskb,
				    struct nf_conntrack_l3proto *l3proto, struct nf_conntrack_l4proto *l4proto);
#endif
#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE) || \
    defined(CONFIG_BCM_NAT)
	struct nf_conn_help *help;
    	unsigned int skip_offload = 0;
#endif

	/* Previously seen (loopback or untracked)?  Ignore. */
	if ((*pskb)->nfct) {
		NF_CT_STAT_INC_ATOMIC(ignore);
		return NF_ACCEPT;
	}

	/* rcu_read_lock()ed by nf_hook_slow */
	l3proto = __nf_ct_l3proto_find((u_int16_t)pf);

	if ((ret = l3proto->prepare(pskb, hooknum, &dataoff, &protonum)) <= 0) {
		DEBUGP("not prepared to track yet or error occured\n");
		return -ret;
	}

#ifdef CONFIG_BCM_NAT
	if ((nf_conntrack_fastnat || nf_conntrack_fastroute) && pf == PF_INET)
	    /* gather fragments before fastpath/fastnat, ipv4 only. */
	    if ((*pskb)->nh.iph->frag_off & htons(IP_MF|IP_OFFSET))
		    if(nf_ct_ipv4_gather_frags(*pskb, hooknum == NF_IP_PRE_ROUTING ? IP_DEFRAG_CONNTRACK_IN : IP_DEFRAG_CONNTRACK_OUT))
			return NF_STOLEN;
#endif

	l4proto = __nf_ct_l4proto_find((u_int16_t)pf, protonum);

	/* It may be an special packet, error, unclean...
	 * inverse of the return code tells to the netfilter
	 * core what to do with the packet. */
	if (l4proto->error != NULL &&
	    (ret = l4proto->error(*pskb, dataoff, &ctinfo, pf, hooknum)) <= 0) {
		NF_CT_STAT_INC_ATOMIC(error);
		NF_CT_STAT_INC_ATOMIC(invalid);
		return -ret;
	}

	ct = resolve_normal_ct(*pskb, dataoff, pf, protonum, l3proto, l4proto,
			       &set_reply, &ctinfo);
	if (!ct) {
		/* Not valid part of a connection */
		NF_CT_STAT_INC_ATOMIC(invalid);
		return NF_ACCEPT;
	}

	if (IS_ERR(ct)) {
		/* Too stressed to deal. */
		NF_CT_STAT_INC_ATOMIC(drop);
		return NF_DROP;
	}

	NF_CT_ASSERT((*pskb)->nfct);

	ret = l4proto->packet(ct, *pskb, dataoff, ctinfo, pf, hooknum);
	if (ret <= 0) {
		/* Invalid: inverse of the return code tells
		 * the netfilter core what to do */
		DEBUGP("nf_conntrack_in: Can't track with proto module\n");
		nf_conntrack_put((*pskb)->nfct);
		(*pskb)->nfct = NULL;
		NF_CT_STAT_INC_ATOMIC(invalid);
		return -ret;
	}

#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE) || defined(CONFIG_BCM_NAT)
	/*
	 * skip ALG marked packets from all fastpaths
	 */
	help = nfct_help(ct);
	if (help && help->helper)
	    skip_offload = 1;

#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE)
	/*
	 * skip ALG and some proto from hardware offload
	 * hw_nat v2 need correct check v6 packets for exclude (nf_conntrack_in called by ipv6_conntrack_in with pf == PF_INET6)
	 * hw_nat v1 not correct support v6 offload - allways skip it by is_local_svc */
#if defined(CONFIG_IPV6) && defined(RA_HW_NAT_IPV6)
	if (((pf == PF_INET && hooknum != NF_IP_LOCAL_OUT) || (pf == PF_INET6 && hooknum != NF_IP6_LOCAL_OUT))
#else
	if (pf == PF_INET && hooknum != NF_IP_LOCAL_OUT
#endif
	    && FOE_ALG(*pskb) == 0 && (skip_offload || is_local_svc(protonum))) {
		FOE_ALG_MARK(*pskb);
	}
#endif
#endif
	/*
	 * full skip not ipv4 and mcast/bcast traffic by software offload and filtering section
	 * allow only established/reply tpc/udp protocol packets for processing in software offload
	 */
	if (FASTNAT_SKIP_PROTO(protonum) || FASTNAT_SKIP_TYPE(*pskb) || (pf != PF_INET))
	    goto skip;

	/* pure route or nat ? */
	pure_route = is_pure_routing(ct);

#ifdef CONFIG_BCM_NAT
	/*
	 * software route offload path
	 * send pkts to fastroute if adress not changed (not nat)
	 */
        if (nf_conntrack_fastroute && !skip_offload && skb_is_ready(*pskb) && pure_route && FASTNAT_ESTABLISHED(ctinfo)) {
    	    int bcmverdict = bcm_do_fastroute(ct, pskb, hooknum, set_reply);
	    /* if accept - continue process in normal path */
    	    if (bcmverdict != NF_ACCEPT)
		return bcmverdict;
	}
#endif
#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE) || defined(CONFIG_BCM_NAT)
	/* this code section may be used for skip some types traffic,
	    only if hardware nat support enabled or software fastnat support enabled */
	if (!pure_route && !skip_offload && (ra_sw_nat_hook_rx != NULL || nf_conntrack_fastnat)) {
#if defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR) || defined(CONFIG_NETFILTER_XT_MATCH_WEBSTR_MODULE)
	    /*  1. skip http post/get/head traffic for correct webstr work */
	    if (web_str_loaded && protonum == IPPROTO_TCP && CTINFO2DIR(ctinfo) == IP_CT_DIR_ORIGINAL) {
		struct tcphdr _tcph, *tcph;
		unsigned char _data[2], *data;

		/* For URL filter; RFC-HTTP: GET, POST, HEAD */
		if ((tcph = skb_header_pointer(*pskb, dataoff, sizeof(_tcph), &_tcph)) &&
		    (data = skb_header_pointer(*pskb, dataoff + tcph->doff*4, sizeof(_data), &_data)) &&
		    (memcmp(data, "GET ", sizeof("GET ")-1) == 0 ||
			memcmp(data, "POST ", sizeof("POST ")-1) == 0 ||
			memcmp(data, "HEAD ", sizeof("HEAD ")-1) == 0)) {
		    skip_offload = 1;
		    goto pass;
		}
	    }
#endif /* XT_MATCH_WEBSTR */
#ifdef CONFIG_NF_CONNTRACK_MARK
	    /* 2. offload only packets with connection mark flag 0 */
	    if ((ct->mark & 0xFF0000) != 0) {
		skip_offload = 1;
		goto pass;
	    }
#endif
	    /* 3. other traffic skip section
		EXAMPLE_CODE:
		if (need ... rules ...) {
		    skip_offload = 1;
		    goto pass;
		}
	    */
pass:
	    if(skip_offload) {
#if defined(CONFIG_RA_HW_NAT) || defined(CONFIG_RA_HW_NAT_MODULE)
		/* skip hardware offload flag */
		if (hooknum != NF_IP_LOCAL_OUT && FOE_ALG(*pskb) == 0)
		    FOE_ALG_MARK(*pskb);
#endif
#ifdef CONFIG_BCM_NAT
		/* skip sofware nat fastpath flag */
		ct->fastnat |= NF_FAST_NAT_DENY;
#endif
		goto skip;
	    }
	}

#ifdef CONFIG_BCM_NAT
	/*
	 * software nat offload path
	 * send pkts to fastroute if adress changed (nat)
	 */
	if (nf_conntrack_fastnat && (hooknum == NF_IP_PRE_ROUTING) &&
	    (FASTNAT_ESTABLISHED(ctinfo) || protonum == IPPROTO_UDP) &&
	    !(ct->fastnat & NF_FAST_NAT_DENY) && !pure_route)
		ret = bcm_do_fastnat(ct, ctinfo, pskb, l3proto, l4proto);
#endif
skip:
#endif /* RA_HW_NAT || BCM_NAT */

	if (set_reply && !test_and_set_bit(IPS_SEEN_REPLY_BIT, &ct->status)) {
#ifdef CONFIG_BCM_NAT
	    if (nf_conntrack_fastnat && pf == PF_INET && hooknum == NF_IP_LOCAL_OUT)
		ct->fastnat |= NF_FAST_NAT_DENY;
#endif
	    nf_conntrack_event_cache(IPCT_STATUS, *pskb);
	}

	return ret;
}
#ifndef CONFIG_SPEEDHACK
EXPORT_SYMBOL_GPL(nf_conntrack_in);
#endif

int FASTPATHNET nf_ct_invert_tuplepr(struct nf_conntrack_tuple *inverse,
			 const struct nf_conntrack_tuple *orig)
{
	int ret;

	rcu_read_lock();
	ret = nf_ct_invert_tuple(inverse, orig,
				 __nf_ct_l3proto_find(orig->src.l3num),
				 __nf_ct_l4proto_find(orig->src.l3num,
						      orig->dst.protonum));
	rcu_read_unlock();
	return ret;
}
EXPORT_SYMBOL_GPL(nf_ct_invert_tuplepr);

/* Alter reply tuple (maybe alter helper).  This is for NAT, and is
   implicitly racy: see __nf_conntrack_confirm */
void FASTPATHNET nf_conntrack_alter_reply(struct nf_conn *ct,
			      const struct nf_conntrack_tuple *newreply)
{
	struct nf_conn_help *help = nfct_help(ct);

	write_lock_bh(&nf_conntrack_lock);
	/* Should be unconfirmed, so not in hash table yet */
	NF_CT_ASSERT(!nf_ct_is_confirmed(ct));

	DEBUGP("Altering reply tuple of %p to ", ct);
	NF_CT_DUMP_TUPLE(newreply);

	ct->tuplehash[IP_CT_DIR_REPLY].tuple = *newreply;
	if (!ct->master && help && help->expecting == 0) {
		struct nf_conntrack_helper *helper;
		helper = __nf_ct_helper_find(newreply);
		if (helper)
			memset(&help->help, 0, sizeof(help->help));
		help->helper = helper;
	}
	write_unlock_bh(&nf_conntrack_lock);
}

/* Refresh conntrack for this many jiffies and do accounting if do_acct is 1 */
void FASTPATHNET __nf_ct_refresh_acct(struct nf_conn *ct,
			  enum ip_conntrack_info ctinfo,
			  const struct sk_buff *skb,
			  unsigned long extra_jiffies,
			  int do_acct)
{
	int event = 0;

	NF_CT_ASSERT(ct->timeout.data == (unsigned long)ct);
	//NF_CT_ASSERT(skb);	// noise

	write_lock_bh(&nf_conntrack_lock);

	/* Only update if this is not a fixed timeout */
	if (test_bit(IPS_FIXED_TIMEOUT_BIT, &ct->status))
		goto acct;

	/* If not in hash table, timer will not be active yet */
	if (!nf_ct_is_confirmed(ct)) {
		ct->timeout.expires = extra_jiffies;
		event = IPCT_REFRESH;
	} else {
		unsigned long newtime = jiffies + extra_jiffies;

		/* Only update the timeout if the new timeout is at least
		   HZ jiffies from the old timeout. Need del_timer for race
		   avoidance (may already be dying). */
		if (newtime - ct->timeout.expires >= HZ
		    && del_timer(&ct->timeout)) {
			ct->timeout.expires = newtime;
			add_timer(&ct->timeout);
			event = IPCT_REFRESH;
		}
	}

acct:
#ifdef CONFIG_NF_CT_ACCT
	if (do_acct) {
		ct->counters[CTINFO2DIR(ctinfo)].packets++;
		ct->counters[CTINFO2DIR(ctinfo)].bytes +=
			skb->len - skb_network_offset(skb);

		if ((ct->counters[CTINFO2DIR(ctinfo)].packets & 0x80000000)
		    || (ct->counters[CTINFO2DIR(ctinfo)].bytes & 0x80000000))
			event |= IPCT_COUNTER_FILLING;
	}
#endif

	write_unlock_bh(&nf_conntrack_lock);

	/* must be unlocked when calling event cache */
	if (event)
		nf_conntrack_event_cache(event, skb);
}
EXPORT_SYMBOL_GPL(__nf_ct_refresh_acct);

#if defined(CONFIG_NF_CT_NETLINK) || defined(CONFIG_NF_CT_NETLINK_MODULE)
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nfnetlink_conntrack.h>
#include <linux/mutex.h>


/* Generic function for tcp/udp/sctp/dccp and alike. This needs to be
 * in ip_conntrack_core, since we don't want the protocols to autoload
 * or depend on ctnetlink */
int nf_ct_port_tuple_to_nfattr(struct sk_buff *skb,
			       const struct nf_conntrack_tuple *tuple)
{
	NFA_PUT(skb, CTA_PROTO_SRC_PORT, sizeof(u_int16_t),
		&tuple->src.u.tcp.port);
	NFA_PUT(skb, CTA_PROTO_DST_PORT, sizeof(u_int16_t),
		&tuple->dst.u.tcp.port);
	return 0;

nfattr_failure:
	return -1;
}
EXPORT_SYMBOL_GPL(nf_ct_port_tuple_to_nfattr);

static const size_t cta_min_proto[CTA_PROTO_MAX] = {
	[CTA_PROTO_SRC_PORT-1]  = sizeof(u_int16_t),
	[CTA_PROTO_DST_PORT-1]  = sizeof(u_int16_t)
};

int nf_ct_port_nfattr_to_tuple(struct nfattr *tb[],
			       struct nf_conntrack_tuple *t)
{
	if (!tb[CTA_PROTO_SRC_PORT-1] || !tb[CTA_PROTO_DST_PORT-1])
		return -EINVAL;

	if (nfattr_bad_size(tb, CTA_PROTO_MAX, cta_min_proto))
		return -EINVAL;

	t->src.u.tcp.port = *(__be16 *)NFA_DATA(tb[CTA_PROTO_SRC_PORT-1]);
	t->dst.u.tcp.port = *(__be16 *)NFA_DATA(tb[CTA_PROTO_DST_PORT-1]);

	return 0;
}
EXPORT_SYMBOL_GPL(nf_ct_port_nfattr_to_tuple);
#endif

/* Used by ipt_REJECT and ip6t_REJECT. */
static void nf_conntrack_attach(struct sk_buff *nskb, struct sk_buff *skb)
{
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;

	/* This ICMP is in reverse direction to the packet which caused it */
	ct = nf_ct_get(skb, &ctinfo);
	if (CTINFO2DIR(ctinfo) == IP_CT_DIR_ORIGINAL)
		ctinfo = IP_CT_RELATED_REPLY;
	else
		ctinfo = IP_CT_RELATED;

	/* Attach to new skbuff, and increment count */
	nskb->nfct = &ct->ct_general;
	nskb->nfctinfo = ctinfo;
	nf_conntrack_get(nskb->nfct);
}

static inline int
do_iter(const struct nf_conntrack_tuple_hash *i,
	int (*iter)(struct nf_conn *i, void *data),
	void *data)
{
	return iter(nf_ct_tuplehash_to_ctrack(i), data);
}

/* Bring out ya dead! */
static struct nf_conn *
get_next_corpse(int (*iter)(struct nf_conn *i, void *data),
		void *data, unsigned int *bucket)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct;
	struct hlist_node *n;

	write_lock_bh(&nf_conntrack_lock);
	for (; *bucket < nf_conntrack_htable_size; (*bucket)++) {
		hlist_for_each_entry(h, n, &nf_conntrack_hash[*bucket], hnode) {
			ct = nf_ct_tuplehash_to_ctrack(h);
			if (iter(ct, data))
				goto found;
		}
	}
	hlist_for_each_entry(h, n, &unconfirmed, hnode) {
		ct = nf_ct_tuplehash_to_ctrack(h);
		if (iter(ct, data))
			set_bit(IPS_DYING_BIT, &ct->status);
	}
	write_unlock_bh(&nf_conntrack_lock);
	return NULL;
found:
	atomic_inc(&ct->ct_general.use);
	write_unlock_bh(&nf_conntrack_lock);
	return ct;
}

void
nf_ct_iterate_cleanup(int (*iter)(struct nf_conn *i, void *data), void *data)
{
	struct nf_conn *ct;
	unsigned int bucket = 0;

	while ((ct = get_next_corpse(iter, data, &bucket)) != NULL) {
		/* Time to push up daises... */
		if (del_timer(&ct->timeout))
			death_by_timeout((unsigned long)ct);
		/* ... else the timer will get him soon. */

		nf_ct_put(ct);
	}
}
EXPORT_SYMBOL_GPL(nf_ct_iterate_cleanup);

static int kill_all(struct nf_conn *i, void *data)
{
	return 1;
}

void ip_conntrack_flush(void)
{
	nf_ct_iterate_cleanup(kill_all, NULL);
}
EXPORT_SYMBOL_GPL(ip_conntrack_flush);

void ip_ct_record_cleanup(void)
{
i_see_dead_people1:
	ip_conntrack_flush();
	if(atomic_read(&nf_conntrack_count)>1){
		schedule();
	goto i_see_dead_people1;
	}
}
EXPORT_SYMBOL(ip_ct_record_cleanup);

static void free_conntrack_hash(struct hlist_head *hash, int vmalloced,
				int size)
{
	if (vmalloced)
		vfree(hash);
	else
		free_pages((unsigned long)hash,
			   get_order(sizeof(struct hlist_head) * size));
}

void nf_conntrack_flush(void)
{
	nf_ct_iterate_cleanup(kill_all, NULL);
}
EXPORT_SYMBOL_GPL(nf_conntrack_flush);

/* Mishearing the voices in his head, our hero wonders how he's
   supposed to kill the mall. */
void nf_conntrack_cleanup(void)
{
	int i;

	rcu_assign_pointer(ip_ct_attach, NULL);

	/* This makes sure all current packets have passed through
	   netfilter framework.  Roll on, two-stage module
	   delete... */
	synchronize_net();

	nf_ct_event_cache_flush();

i_see_dead_people:
	nf_conntrack_flush();

	if (atomic_read(&nf_conntrack_count) != 0) {
		schedule();
		goto i_see_dead_people;
	}

	/* wait until all references to nf_conntrack_untracked are dropped */
	while (atomic_read(&nf_conntrack_untracked.ct_general.use) > 1)
		schedule();

	rcu_assign_pointer(nf_ct_destroy, NULL);

	for (i = 0; i < NF_CT_F_NUM; i++) {
		if (nf_ct_cache[i].use == 0)
			continue;

		NF_CT_ASSERT(nf_ct_cache[i].use == 1);
		nf_ct_cache[i].use = 1;
		nf_conntrack_unregister_cache(i);
	}
	kmem_cache_destroy(nf_conntrack_expect_cachep);
	free_conntrack_hash(nf_conntrack_hash, nf_conntrack_vmalloc,
			    nf_conntrack_htable_size);

	nf_conntrack_l4proto_unregister(&nf_conntrack_l4proto_generic);

	/* free l3proto protocol tables */
	for (i = 0; i < PF_MAX; i++)
		if (nf_ct_protos[i]) {
			kfree(nf_ct_protos[i]);
			nf_ct_protos[i] = NULL;
		}
}

static struct hlist_head *alloc_hashtable(int *sizep, int *vmalloced)
{
	struct hlist_head *hash;
	unsigned int size, i;

	*vmalloced = 0;
	size = *sizep = roundup(*sizep, PAGE_SIZE / sizeof(struct hlist_head));
	hash = (void*)__get_free_pages(GFP_KERNEL|__GFP_NOWARN,
 				       get_order(sizeof(struct hlist_head)
 						 * size));
	if (!hash) {
		*vmalloced = 1;
		printk(KERN_WARNING "nf_conntrack: falling back to vmalloc.\n");
		hash = vmalloc(sizeof(struct hlist_head) * size);
	}

	if (hash)
		for (i = 0; i < size; i++)
			INIT_HLIST_HEAD(&hash[i]);

	return hash;
}

int nf_conntrack_set_hashsize(const char *val, struct kernel_param *kp)
{
	int i, bucket, vmalloced, old_vmalloced;
	unsigned int hashsize, old_size;
	int rnd;
	struct hlist_head *hash, *old_hash;
	struct nf_conntrack_tuple_hash *h;

	/* On boot, we can set this without any fancy locking. */
	if (!nf_conntrack_htable_size)
		return param_set_uint(val, kp);

	hashsize = simple_strtoul(val, NULL, 0);
	if (!hashsize)
		return -EINVAL;

	hash = alloc_hashtable(&hashsize, &vmalloced);
	if (!hash)
		return -ENOMEM;

	/* We have to rehahs for the new table anyway, so we also can
	 * use a newrandom seed */
	get_random_bytes(&rnd, 4);

	write_lock_bh(&nf_conntrack_lock);
	for (i = 0; i < nf_conntrack_htable_size; i++) {
		while (!hlist_empty(&nf_conntrack_hash[i])) {
			h = hlist_entry(nf_conntrack_hash[i].first,
					struct nf_conntrack_tuple_hash, hnode);
			hlist_del(&h->hnode);
			bucket = __hash_conntrack(&h->tuple, hashsize, rnd);
			hlist_add_head(&h->hnode, &hash[bucket]);
		}
	}
	old_size = nf_conntrack_htable_size;
	old_vmalloced = nf_conntrack_vmalloc;
	old_hash = nf_conntrack_hash;

	nf_conntrack_htable_size = hashsize;
	nf_conntrack_vmalloc = vmalloced;
	nf_conntrack_hash = hash;
	nf_conntrack_hash_rnd = rnd;
	write_unlock_bh(&nf_conntrack_lock);

	free_conntrack_hash(old_hash, old_vmalloced, old_size);
	return 0;
}
EXPORT_SYMBOL_GPL(nf_conntrack_set_hashsize);

module_param_call(hashsize, nf_conntrack_set_hashsize, param_get_uint,
		  &nf_conntrack_htable_size, 0600);

s16 (*nf_ct_nat_offset)(const struct nf_conn *ct,
			enum ip_conntrack_dir dir,
			u32 seq);
EXPORT_SYMBOL_GPL(nf_ct_nat_offset);

void nf_ct_untracked_status_or(unsigned long bits)
{
	nf_conntrack_untracked.status |= bits;
}
EXPORT_SYMBOL_GPL(nf_ct_untracked_status_or);

int __init nf_conntrack_init(void)
{
	unsigned int i;
#if 0
	int max_factor = 8;
#endif
	int ret;

#ifdef CONFIG_NAT_CONE
	nf_conntrack_nat_mode = NAT_MODE_LINUX;
#endif
#ifdef CONFIG_BCM_NAT
	nf_conntrack_fastnat = 1;
	nf_conntrack_fastroute = 1;
#endif

	/* Idea from tcp.c: use 1/16384 of memory.  On i386: 32MB
	 * machine has 512 buckets.  >= 1GB machines have 8192 buckets. */
#if 0
	if (!nf_conntrack_htable_size) {
		nf_conntrack_htable_size
			= (((totalram_pages << PAGE_SHIFT) / 16384)
			   / sizeof(struct hlist_head));
		if (totalram_pages > (1024 * 1024 * 1024 / PAGE_SIZE))
			nf_conntrack_htable_size = 16384;
		if (nf_conntrack_htable_size < 32)
			nf_conntrack_htable_size = 32;

		/* Use a max. factor of four by default to get the same max as
		 * with the old struct list_heads. When a table size is given
		 * we use the old value of 8 to avoid reducing the max.
		 * entries. */
		max_factor = 4;
	}

	nf_conntrack_max = max_factor * nf_conntrack_htable_size;
#endif
	 /* SpeedMod: Hashtable size: Limit ~2k records for 16Mb devices and ~4k for 32Mb.
	    This is must be set depended at foe table size. And may be changed by /proc */
#if defined(CONFIG_RAETH_MEMORY_OPTIMIZATION) || defined(CONFIG_RT2860V2_AP_MEMORY_OPTIMIZATION) || \
    defined(CONFIG_RA_HW_NAT_TBL_1K) || defined(CONFIG_RA_HW_NAT_TBL_2K)
        nf_conntrack_htable_size = 2048;
#else
#if defined(CONFIG_RA_HW_NAT_TBL_4K)
        nf_conntrack_htable_size = 4096;
#elif defined(CONFIG_RA_HW_NAT_TBL_8K)
        nf_conntrack_htable_size = 8192;
#elif defined(CONFIG_RA_HW_NAT_TBL_16K)
        nf_conntrack_htable_size = 16384;
#else
        nf_conntrack_htable_size = 4096;
#endif
#endif
        nf_conntrack_max = nf_conntrack_htable_size * 2;
	nf_conntrack_hash = alloc_hashtable(&nf_conntrack_htable_size,
					    &nf_conntrack_vmalloc);
	if (!nf_conntrack_hash) {
		printk(KERN_ERR "Unable to create nf_conntrack_hash\n");
		goto err_out;
	}

	printk("nf_conntrack version %s (%u buckets, %d max)\n",
	       NF_CONNTRACK_VERSION, nf_conntrack_htable_size,
	       nf_conntrack_max);

	ret = nf_conntrack_register_cache(NF_CT_F_BASIC, "nf_conntrack:basic",
					  sizeof(struct nf_conn));
	if (ret < 0) {
		printk(KERN_ERR "Unable to create nf_conn slab cache\n");
		goto err_free_hash;
	}

	nf_conntrack_expect_cachep = kmem_cache_create("nf_conntrack_expect",
					sizeof(struct nf_conntrack_expect),
					0, 0, NULL, NULL);
	if (!nf_conntrack_expect_cachep) {
		printk(KERN_ERR "Unable to create nf_expect slab cache\n");
		goto err_free_conntrack_slab;
	}

	ret = nf_conntrack_l4proto_register(&nf_conntrack_l4proto_generic);
	if (ret < 0)
		goto out_free_expect_slab;

	for (i = 0; i < AF_MAX; i++)
		nf_ct_l3protos[i] = &nf_conntrack_l3proto_generic;

	/* For use by REJECT target */
	rcu_assign_pointer(ip_ct_attach, nf_conntrack_attach);
	rcu_assign_pointer(nf_ct_destroy, destroy_conntrack);

	/* Howto get NAT offsets */
	rcu_assign_pointer(nf_ct_nat_offset, NULL);

	/* Set up fake conntrack:
	    - to never be deleted, not in any hashes */
	atomic_set(&nf_conntrack_untracked.ct_general.use, 1);
	/*  - and look it like as a confirmed connection */
	nf_ct_untracked_status_or(IPS_CONFIRMED | IPS_UNTRACKED);

	return ret;

out_free_expect_slab:
	kmem_cache_destroy(nf_conntrack_expect_cachep);
err_free_conntrack_slab:
	nf_conntrack_unregister_cache(NF_CT_F_BASIC);
err_free_hash:
	free_conntrack_hash(nf_conntrack_hash, nf_conntrack_vmalloc,
			    nf_conntrack_htable_size);
err_out:
	return -ENOMEM;
}
