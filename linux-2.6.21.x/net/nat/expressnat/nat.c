

#ifdef MODULE
#define __NO_VERSION__
#endif
#include <linux/version.h>
#include <linux/config.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <net/checksum.h>
#include <linux/stddef.h>
#include <linux/sysctl.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <net/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/list.h>
#include <asm/semaphore.h>
#include <linux/delay.h>

#include <linux/netfilter_ipv4/ip_conntrack.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>

MODULE_AUTHOR("nikicankik@gmail.com");
MODULE_DESCRIPTION("Fast Path Nat kernel module.");
MODULE_LICENSE("GPL");

/* Config options */

#define PRINT_NAT_TABLE             /* Proc table dumping support */
#define DEBUG                       /* Debug messages */
#define TIMEOUTS                    /* Compile connection timeout support */
#define OPTIMISE_FOR_SHORT_CONNECTIONS /* Static timeout */
//#define OPTIMISE_FOR_LONG_CONNECTIONS /* Timeout refreshed */
//#define ALG_COMPATIBLE              /* Consult Conntrack database for ALG */
#define NO_XLR8_RELATED 0          /*Should Related connections be xlr8ed?*/


#ifdef DEBUG
#define DEBUGP printk
//#define inline
#else
#define DEBUGP(format, args...)
#endif

#ifdef TIMEOUTS
    #ifdef OPTIMISE_FOR_SHORT_CONNECTIONS
        #define NAT_TIMEOUT (180 * HZ)
        /* No Refresh for every packet */
        #define nat_timeout_refresh(x)
    #else /* OPTIMISE_FOR_LONG_CONNECTIONS */
        #define NAT_TIMEOUT (5 * HZ)
        #define nat_timeout_refresh(x) mod_timer(x, jiffies + NAT_TIMEOUT) 
    #endif /* OPTIMISE_FOR_SHORT_CONNECTIONS */
#else /* ! TIMEOUTS */
    #define NAT_TIMEOUT 0 
    #define nat_timeout_refresh(x)
#endif /* TIMEOUTS */

#ifdef ALG_COMPATIBLE
#define ALG_IS_REGD(x) alg_is_registered(x)
#else
#define ALG_IS_REGD(x) 0
#endif /* ALG_COMPATIBLE */

#define SRC_PORT      1
#define DST_PORT      2
#define PORT_INVALID  0



#define ip_hdr(skb)		(struct iphdr *)skb->data

#define MAGIC_COOKIE    0xdeadbeef

#define IS_ICMP(skb) (((struct iphdr*) skb->nh.iph)->protocol == IPPROTO_ICMP)
#define IS_TCP(skb) (((struct iphdr*) skb->nh.iph)->protocol ==  IPPROTO_TCP )
#define IS_UDP(skb) (((struct iphdr*) skb->nh.iph)->protocol ==  IPPROTO_UDP )

#define IS_NOT_LOCAL(skb) (skb->sk == NULL)

#define IS_FRAG(skb) ((skb->nh.iph->frag_off == 0) || \
                        (skb->nh.iph->frag_off == 1<<14)) 

#define IS_NOT_FRAG(skb) ! IS_FRAG(skb)

#define WRITE_HASH_LOCK(x)         write_lock_bh(x)
#define WRITE_HASH_UNLOCK(x)       write_unlock_bh(x)
#define READ_HASH_LOCK(x)          read_lock_bh(x)
#define READ_HASH_UNLOCK(x)        read_unlock_bh(x)

#define MATCH_TYPE uint8_t

#define MATCH_TYPE_NOMATCH 0
#define MATCH_TYPE_EXACT   1
#define MATCH_TYPE_APPROX  2

static DEFINE_RWLOCK(hash_table_lock);
 
const uint32_t max_hash_size = 1024;

extern int ip_finish_output2(struct sk_buff *skb);

/* Packet info structure (stored at the bottom of the packet buffer) */
typedef struct {
    uint32_t                srcIp;
    uint32_t                dstIp;
    uint16_t                srcPort;
    uint16_t                dstPort;
    struct net_device       *dev; /* Pointer to device we will leave by */
    struct dst_entry        *dst;
    uint8_t                 tos;
} PacketInfo;

struct hash_table {
	struct list_head list;		/* list of clashing entries in hash */
	PacketInfo pre;			    /* packet info before*/
	PacketInfo post;			/* packet info after */
	struct timer_list timeout;  /* Timeout */    
};

struct hash_table *global_set_hash = NULL;

#ifdef TIMEOUTS
static void delete_nat_entry(unsigned long ul_set_hash)
{
	struct hash_table *set_hash = (void *)ul_set_hash;

	WRITE_HASH_LOCK(&hash_table_lock);
	list_del(&set_hash->list);
    kfree(set_hash);
	WRITE_HASH_UNLOCK(&ip_conntrack_lock);
}

#endif /* TIMEOUTS */


#ifdef PRINT_NAT_TABLE

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *proc_net_fast_nat = NULL;
static struct proc_dir_entry *proc_net_ipt_nat_status = NULL;

static int
proc_nat_status_entry_show (struct seq_file *s, void *v)
    {
    struct hash_table *node=NULL,*tmp_item;
    uint32_t key;
        seq_printf(s, 
                   "    OrigSrcIP     OrigSrcPort          OrigEndIP    OrigDstPort "
                   "      NatSrcIP      NatSrcPort           NatDstIP     NatDstPort  Timeout"
                   "\n\n");

    READ_HASH_LOCK(&hash_table_lock);
    for (key = 0; key < max_hash_size; key++)
        {
        list_for_each_entry_safe(node,tmp_item, 
                                 &global_set_hash[key].list, list)
            {
            seq_printf(s, "  %u.%u.%u.%u       %u        %u.%u.%u.%u       %u        "
                          "  %u.%u.%u.%u       %u        %u.%u.%u.%u       %u     %ul\n",
                        NIPQUAD(node->pre.srcIp), ntohs(node->pre.srcPort),
                        NIPQUAD(node->pre.dstIp), ntohs(node->pre.dstPort),
                        NIPQUAD(node->post.srcIp), ntohs(node->post.srcPort),
                        NIPQUAD(node->post.dstIp), ntohs(node->post.dstPort),
                        (uint32_t)((node->timeout.expires - jiffies) / HZ));
            }
        }
    READ_HASH_UNLOCK(&hash_table_lock);
    return 0;    
    }

static void *single_start(struct seq_file *p, loff_t *pos)
{
        return NULL + (*pos == 0);
}

static void *single_next(struct seq_file *p, void *v, loff_t *pos)
{
        ++*pos;
        return NULL;
}

static void single_stop(struct seq_file *p, void *v)
{
}


inline int nat_single_open(struct file *file, 
                       int (*show)(struct seq_file *, void *), void *data)
{
        struct seq_operations *op = kmalloc(sizeof(*op), GFP_KERNEL);
        int res = -ENOMEM;

        if (op) {
                op->start = single_start;
                op->next = single_next;
                op->stop = single_stop;
                op->show = show;
                res = seq_open(file, op);
                if (!res)
                        ((struct seq_file *)file->private_data)->private = data;
                else
                        kfree(op);
        }
        return res;
}

inline int nat_single_release(struct inode *inode, struct file *file)
{
        const struct seq_operations *op = 
                        ((struct seq_file *)file->private_data)->op;
        int res = seq_release(inode, file);
        kfree(op);
        return res;
}

static int proc_nat_status_open (struct inode *inode, struct file *file)
    {
    int  ret=0;
    ret  = nat_single_open(file, proc_nat_status_entry_show, NULL);
    if (!ret) seq_lseek(file, 0, 0);    
    return (ret);
    }

static int proc_nat_status_release (struct inode *inode, struct file *file)
    {
    return (seq_release(inode, file));
    }

static struct file_operations proc_nat_status_file_ops = {
  .owner = THIS_MODULE,
  .open = proc_nat_status_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = proc_nat_status_release
};
#endif /* PRINT_NAT_TABLE */

/* Calculate hash from PacketInfo*/

uint32_t inline hashfromkey(PacketInfo *info)
    {
    uint32_t key;

    key = (((info->srcIp << 17) | (info->srcIp >> 15)) ^ info->dstIp) +
            (info->srcPort * 17) + (info->dstPort * 13 * 29);

    key += ~(key << 9);
    key ^=  ((key >> 14) | (key << 18));
    key +=  (key << 4);
    key ^=  ((key >> 10) | (key << 22));

    return key % max_hash_size;
    }

/* Get Port number from packet */

static inline uint16_t
getPort(const struct sk_buff *skb, uint16_t *ports)
{
	struct iphdr *iph = (struct iphdr * )skb->data;

    switch (iph->protocol) 
        {
	    case IPPROTO_TCP: 
            {
	        struct tcphdr *tcph = (struct tcphdr *) (skb->data + 
                    (iph->ihl << 2));
             	
	     	ports[SRC_PORT] = tcph->source; 
            ports[DST_PORT] = tcph->dest;

            return 0;
	        }

	case IPPROTO_UDP: 
            {
	        struct udphdr *udph = (struct udphdr *) (skb->data + 
                    (iph->ihl << 2));
             	
	     	ports[SRC_PORT] = udph->source; 
            ports[DST_PORT] = udph->dest;
            return 0;
	        }
	default:
			ports[SRC_PORT] = ports[DST_PORT] = PORT_INVALID;
            return -1;
	}
}

static inline uint16_t
setPorts(const struct sk_buff *skb, PacketInfo *info)
{
	struct iphdr *iph = (struct iphdr * )skb->data;

    switch (iph->protocol) 
        {
	    case IPPROTO_TCP: 
            {
	        struct tcphdr *tcph = (struct tcphdr *) (skb->data + 
                    (iph->ihl << 2));
            
	     	tcph->source = info->srcPort; 
            tcph->dest  = info->dstPort;
            break;
	        }
	    case IPPROTO_UDP: 
            {
	        struct udphdr *udph = (struct udphdr *) (skb->data + 
                    (iph->ihl << 2));
             	
	     	udph->source = info->srcPort;  
            udph->dest   = info->dstPort;
            break;
	        }
	    default:
            break;
	}
    return 0;
}


/* Find an entry in hash table */

static inline struct hash_table *
hash_table_find (PacketInfo *p, uint32_t key, MATCH_TYPE * matchType)
    {
    struct hash_table *set_hash = NULL;

    if(!p)
        {
        *matchType = MATCH_TYPE_NOMATCH;
        goto out;
        }

    READ_HASH_LOCK(&hash_table_lock);
    
    if(list_empty(&global_set_hash[key].list))
        {
        *matchType = MATCH_TYPE_NOMATCH;
        goto unlock;
        }
    
    *matchType = MATCH_TYPE_APPROX;
    
    list_for_each_entry(set_hash, &global_set_hash[key].list, list)
        {
        if(set_hash)
            {
            if  (set_hash->pre.srcIp   == p->srcIp   && 
                 set_hash->pre.dstIp   == p->dstIp   && 
                 set_hash->pre.srcPort == p->srcPort &&
                 set_hash->pre.dstPort == p->dstPort
                )
                {
                *matchType = MATCH_TYPE_EXACT;
                goto unlock;
                }
            }
        }

unlock:
    READ_HASH_UNLOCK(&hash_table_lock);
out:
    return set_hash;            
    }

/* Hash table add */

static inline int hash_table_add (PacketInfo *pre, PacketInfo *post, struct sk_buff *skb)
{

    struct hash_table *set_hash;
    int ret = 0;
    uint32_t key ;
    MATCH_TYPE matchType;

    if(!pre || !post)
        return -EINVAL;

    if(!strcmp(pre->dev->name, "lo")  || 
       !strcmp(post->dev->name,"lo") || 
       !strcmp(pre->dev->name , post->dev->name))
            {
            return -EINVAL;
            }

    DEBUGP(KERN_CRIT"Adding to table %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u\n",
                        NIPQUAD(pre->srcIp),ntohs(pre->srcPort),
                        NIPQUAD(pre->dstIp),ntohs(pre->dstPort));
    DEBUGP(KERN_CRIT"Adding to table %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u\n",
                        NIPQUAD(post->srcIp),ntohs(post->srcPort),
                        NIPQUAD(post->dstIp),ntohs(post->dstPort));
#ifdef DRY_RUN
    return 0;
#endif /* DRY_RUN */

    key =  hashfromkey(pre);

	set_hash = hash_table_find(pre, key, &matchType);


	if (matchType != MATCH_TYPE_EXACT ) 
        {
        set_hash = kmalloc(sizeof(struct hash_table),GFP_ATOMIC);

		if (!set_hash) 
            {
			ret = -ENOMEM;
			goto unlock;
		    }
		INIT_LIST_HEAD(&set_hash->list);
        memcpy(&set_hash->pre,pre,sizeof(PacketInfo));
        memcpy(&set_hash->post,post,sizeof(PacketInfo));

#ifdef TIMEOUTS
        init_timer(&set_hash->timeout);   
        set_hash->timeout.function = delete_nat_entry; 
        set_hash->timeout.data     = (unsigned long) set_hash;
        set_hash->timeout.expires  = jiffies + (60 * HZ);
        add_timer(&set_hash->timeout);
#endif /* TIMEOUTS */

	    WRITE_HASH_LOCK(&hash_table_lock);
		list_add(&set_hash->list, &global_set_hash[key].list);
    	WRITE_HASH_UNLOCK(&hash_table_lock);
	    }     
#if 0
    else   /* if(matchType == MATCH_TYPE_EXACT) */
        {
        DEBUGP(KERN_CRIT "where did this packet come from ?,deleting !\n");
	    //WRITE_HASH_LOCK(&hash_table_lock);
        //list_del(&set_hash->list);
	    //WRITE_HASH_UNLOCK(&hash_table_lock);
        }
#endif
unlock:
	return ret;
}


/* Does the nat */

void inline do_nat(struct sk_buff *skb , PacketInfo *post)
    {
    struct iphdr *iph = (struct iphdr *)skb->data;
	uint16_t	check;

    u32 odaddr = iph->daddr;
	u32 osaddr = iph->saddr;

    /* increase reference count ? */

    skb->dev    = post->dev;
    skb->dst    = dst_clone (post->dst);

    /* Rewrite IP header */
	iph->daddr = post->dstIp;
	iph->saddr = post->srcIp;
	iph->tos =   post->tos;
	iph->check = 0;
	iph->ttl  -= 1;  

    //setPorts(skb,post);

	iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
   
	if (!(iph->frag_off & htons(IP_OFFSET))) 
        {
		u16	*cksum;

		switch(iph->protocol) 
            {
		    case IPPROTO_TCP:
			    cksum  = (u16*)&((struct tcphdr*)(((char*)iph) + 
                                              (iph->ihl<<2)))->check;
			    if ((u8*)(cksum+1) > skb->tail)
                    break;
			    check = *cksum;
			    if (skb->ip_summed != CHECKSUM_PARTIAL)
				    check = ~check;
			    check = csum_tcpudp_magic(iph->saddr, iph->daddr, 0, 0, check);
			    check = csum_tcpudp_magic(~osaddr, ~odaddr, 0, 0, ~check);
			    if (skb->ip_summed == CHECKSUM_PARTIAL)
				    check = ~check;
			    *cksum = check;

			break;

		case IPPROTO_UDP:
			cksum  = (u16*)&((struct udphdr*)(((char*)iph) + 
                                              (iph->ihl<<2)))->check;
			if ((u8*)(cksum+1) > skb->tail)
				break;
			if ((check = *cksum) != 0) 
                {
				check = csum_tcpudp_magic(iph->saddr, iph->daddr, 0, 0, ~check);
				check = csum_tcpudp_magic(~osaddr, ~odaddr, 0, 0, ~check);
				*cksum = check ? : 0xFFFF;
			    }
			break;
        default:
            break;

        	}
	    }
    }


/* Fill PacketInfo struct from skbuff*/

void inline skb_to_info(struct sk_buff *skb , PacketInfo *info)
    {
    struct iphdr *ip = (struct iphdr*) skb->nh.iph;
    uint16_t ports[DST_PORT+1];
    
    info->srcIp  = ip->saddr;
    info->dstIp  = ip->daddr;
    info->tos    = ip->tos;
    info->dev    = skb->dev;
    info->dst    = skb->dst;
    getPort(skb,&ports[0]);
    info->srcPort = ports[SRC_PORT]; 
    info->dstPort = ports[DST_PORT];
    }

/* Get PacketInfo from marked skbuff*/

void inline info_from_skb_mark_get(struct sk_buff *skb , PacketInfo **info)
    {
    /* This data is aligned on 8 byte boundary. */
    *(info) = (PacketInfo *) ((((uint32_t)skb->end +
                    sizeof(struct skb_shared_info)) + 7) & ~7u);
    }

/* add all info we need */
void inline mark_packet_early(struct sk_buff *skb)
    {
    skb->mark = MAGIC_COOKIE;
    }

/* add all info we need */
void inline mark_packet_late(struct sk_buff *skb)
    {
    PacketInfo * info;
    info_from_skb_mark_get(skb , &info);
    skb_to_info(skb,info); 
    }

/* Fast nat decision ? */
int inline trafficSelect(struct sk_buff *skb)
    {
    /*if(strcmp(skb->dev->name,"lo"))*/
    if(IS_NOT_FRAG(skb) )
            return (IS_TCP(skb));
    return 0;
    }

int inline trafficSelectPre(struct sk_buff *skb)
    {
    return (IS_NOT_FRAG(skb) && ! IS_ICMP(skb));
    }

int inline isPacketMarked(struct sk_buff *skb)
    {
    /* Usual flag checks are of type return (skb->mark & MAGIC_COOKIE)
     * but here we dont want to fiddle with packets that other parts of
     * firewall and kernel code is explicitly marking
     */
    return (skb->mark == MAGIC_COOKIE);
    }

int inline alg_is_registered(struct sk_buff *skb)
    {
    struct ip_conntrack *ct;
    int ret = 0;

   	if (skb->nfct)
        { 
        ct = (struct ip_conntrack *)skb->nfct;

        if(ct->master)
            {
            DEBUGP(KERN_CRIT "Related Connection\n");
            ret = NO_XLR8_RELATED;
            }
        else if(ct->helper)
            {
            DEBUGP(KERN_CRIT "Master\n");
            ret = 1;
            }
        }
    return ret;
    }

/* pre-routing hook */
static unsigned int
conntrack_hook_pre_early(unsigned int hooknum, struct sk_buff **pskb,
               const struct net_device *indev, const
               struct net_device *outdev, int
               (*okfn)(struct sk_buff *))
    {
    PacketInfo pre;
    struct hash_table *nat;
    MATCH_TYPE matchType;
    uint32_t key;

    /* Cannot remove this or gives kernel panic 
     * XXX: Maybe we can write a trafficSelectEarly that is quick 
     */

    if(trafficSelectPre(*pskb) == 0)
        goto pre_routing_done;

    skb_to_info(*pskb,&pre);

    key = hashfromkey(&pre);

    nat=hash_table_find(&pre,key,&matchType);

    if(matchType==MATCH_TYPE_EXACT)
        {
        /* Increase timeout expire */
        nat_timeout_refresh(&nat->timeout);
        do_nat((*pskb),&nat->post);
        
        if(ip_finish_output2(*pskb) == 0)
            {
            return NF_STOLEN;
            }
        else
            {
            /* We are dropping to free resource. */
            return NF_DROP;
            }
        }
    else
        {
        mark_packet_early(*pskb);
        mark_packet_late(*pskb);        
        }
pre_routing_done:
    /* We are not filtering*/
    return NF_ACCEPT;
    }


/* post-routing hook */
static unsigned int
conntrack_hook_post(unsigned int hooknum, struct sk_buff **pskb,
               const struct net_device *indev, const
               struct net_device *outdev, int
               (*okfn)(struct sk_buff *))
    {
#if 0 
    /*We dont need this coz the mark is enough to tell us if traffic is 
        selected.
    */

    if(trafficSelect == 0)
        goto post_routing_done; 
#endif

    if(isPacketMarked(*pskb))
        {
        PacketInfo post;
        PacketInfo *pre ;
        info_from_skb_mark_get ((*pskb),&pre);
        skb_to_info(*pskb, &post);
        if(ALG_IS_REGD(*pskb) == 0)
            hash_table_add(pre,&post,(*pskb));
        else
            DEBUGP(KERN_CRIT "ALG test failed - not adding\n");
        }
    return NF_ACCEPT;
    }

/* Pre routing hook to mark packet */
static struct nf_hook_ops conntrack_ops_pre_early
=   { 
        { NULL, NULL }, 
        conntrack_hook_pre_early,
        THIS_MODULE,
        PF_INET, 
        NF_IP_PRE_ROUTING,
        NF_IP_PRI_FIRST,  /* We come first*/
    };


/* Post routing hook to print the mark */
static struct nf_hook_ops conntrack_ops_post
=   { 
        { NULL, NULL }, 
        conntrack_hook_post,
        THIS_MODULE,
        PF_INET, 
        NF_IP_POST_ROUTING,
        NF_IP_PRI_LAST, /* We are last in post routing */
    };

static int __init init(void)
    {
    uint32_t key;

#ifdef PRINT_NAT_TABLE
    /* Create Proc Dir /proc/net/fast_nat */

    proc_net_fast_nat = proc_mkdir("fast_nat", proc_net);
    if (!proc_net_fast_nat) {		
        printk(KERN_INFO "failed to create procfs entry .\n");
        return (-ENOMEM);
    }
    proc_net_fast_nat->owner = THIS_MODULE;

    /* Create /proc/net/fast_nat/entries */
    proc_net_ipt_nat_status = create_proc_entry ("entries", 0644,
                                                proc_net_fast_nat);

    if (proc_net_ipt_nat_status) 
        {
        proc_net_ipt_nat_status->proc_fops = &proc_nat_status_file_ops;
        proc_net_ipt_nat_status->owner = THIS_MODULE;
        }
#endif /* PRINT_NAT_TABLE */

    global_set_hash = kmalloc(sizeof(struct hash_table) * max_hash_size,
                              GFP_KERNEL);
    DEBUGP(KERN_CRIT "Allocating hash mem\n");

    for (key = 0; key < max_hash_size; key++)
		INIT_LIST_HEAD(&global_set_hash[key].list);

    DEBUGP(KERN_CRIT "Registering hooks\n");

    nf_register_hook(&conntrack_ops_pre_early);

    return nf_register_hook(&conntrack_ops_post);
    }

static void __exit fini(void)
    {
    uint32_t key=0;
    struct hash_table *set_hash = NULL , *temp;

#ifdef PRINT_NAT_TABLE
	remove_proc_entry("fast_nat", proc_net);
	remove_proc_entry("entries", proc_net_fast_nat);
#endif /* PRINT_NAT_TABLE */

    nf_unregister_hook(&conntrack_ops_post);
    nf_unregister_hook(&conntrack_ops_pre_early);

	WRITE_HASH_LOCK(&hash_table_lock);

    for (key = 0; key < max_hash_size; key++)
		list_for_each_entry_safe(set_hash,temp, 
                                 &global_set_hash[key].list, list)
        {
#ifdef TIMEOUTS
        del_timer(&set_hash->timeout);
#endif /*TIMEOUTS*/
        list_del(&set_hash->list);
        kfree(set_hash);
        }
    DEBUGP(KERN_CRIT "Freeing Global DB\n");
    kfree(global_set_hash);
	WRITE_HASH_UNLOCK(&hash_table_lock);
    }

module_init(init);
module_exit(fini);

