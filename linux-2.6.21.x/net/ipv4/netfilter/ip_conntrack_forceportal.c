#include <linux/init.h>
#include <asm/system.h>
#include <linux/sched.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/un.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/config.h>
#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/route.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/if.h>
#include <linux/spinlock.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#include <linux/netfilter_bridge.h>
#endif

#if 0
#include "../../bridge/br_private.h"  //for br_get_all_addr
#endif

/*
	notice: if you want change the redirect or Mac information , 
		you must turn off enableForce, after you set, you 
		can tuen on the enableForce, or race may be happen
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("huanghongbo <huanghongbo@twsz.com>");
MODULE_DESCRIPTION("foce portal module");
#if 0
#define DEBUGP printk
#define DUMP_CONTENT(dptr, length) \
	{\
	  int i;\
	  DEBUGP("\n*************************DUMP***********************\n");\
	  for (i=0;i<length;i++)\
	    DEBUGP("%c",dptr[i]);\
	  DEBUGP("*************************DUMP OVER******************\n");\
	}
#define DUMP_TUPLE_TCP(tp) \
	DEBUGP("tuple(tcp) %p: %u %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u\n",	\
	       (tp), (tp)->dst.protonum,				\
	       NIPQUAD((tp)->src.ip), ntohs((tp)->src.u.tcp.port),		\
	       NIPQUAD((tp)->dst.ip), ntohs((tp)->dst.u.tcp.port))
#else
#define DEBUGP(format, args...)
#define DUMP_CONTENT(dptr, length) 
#define DUMP_TUPLE_TCP(tp) 
#endif

enum EM_FORCE_DEV_TYPE {
	EM_FORCE_DEV_PC = 0,
	EM_FORCE_DEV_STB,
	EM_FORCE_DEV_MOBILE
};

#define ENABLE_DEVICE
#define MAX_LIST_SIZE 64
#define HTTP_PORT 80

atomic_t enableForce; 
static int device_enable[3] = {0,0,0};
static char pPortalPC[50] ;
static char pPortalSTB[50] ;
static char pPortalMobile[50] ;
//static ushort http_port_array[MAX_LIST_SIZE] = { [0 ... (MAX_LIST_SIZE -1)] = 0 };


static inline struct rtable *route_reverse(struct sk_buff *skb, int hook)
{
        struct iphdr *iph = skb->nh.iph;
        struct dst_entry *odst;
        struct flowi fl = {};
        struct rtable *rt;

        /* We don't require ip forwarding to be enabled to be able to
         * send a RST reply for bridged traffic. */
        if (hook != NF_IP_FORWARD
#ifdef CONFIG_BRIDGE_NETFILTER
            || (skb->nf_bridge && skb->nf_bridge->mask & BRNF_BRIDGED)
#endif
           ) {
                fl.nl_u.ip4_u.daddr = iph->saddr;
                if (hook == NF_IP_LOCAL_IN)
                        fl.nl_u.ip4_u.saddr = iph->daddr;
                fl.nl_u.ip4_u.tos = RT_TOS(iph->tos);

                if (ip_route_output_key(&rt, &fl) != 0)
                        return NULL;
        } else {
                /* non-local src, find valid iif to satisfy
                 * rp-filter when calling ip_route_input. */
                fl.nl_u.ip4_u.daddr = iph->daddr;
                if (ip_route_output_key(&rt, &fl) != 0)
                        return NULL; 

                odst = skb->dst;
                if (ip_route_input(skb, iph->saddr, iph->daddr,
                                   RT_TOS(iph->tos), rt->u.dst.dev) != 0) {
                        dst_release(&rt->u.dst);
                        return NULL;
                }
                dst_release(&rt->u.dst);
                rt = (struct rtable *)skb->dst;
                skb->dst = odst;
        }
 
      if (rt->u.dst.error) {
                dst_release(&rt->u.dst);
                rt = NULL;
        }

        return rt;
}


char* pszHttpRedirectHead = 
	"HTTP/1.1 302 Object Moved\r\n"
	"Location: http://%s\r\n"
	"Server: adsl-router-gateway\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: %d\r\n"
	"\r\n"
	"%s";
char* pszHttpRedirectContent = 
	"<html><head><title>Object Moved</title></head>"
	"<body><h1>Object Moved</h1>This Object may be found in "
	"<a HREF=\"http://%s\">here</a>.</body><html>";
 
/* Send http redirect response */
static void send_redirect(struct sk_buff *oldskb, int hook, int devtype)
{
	struct sk_buff *nskb;
	struct tcphdr otcph, *tcph;
	u_int16_t tmp_port;
	u_int32_t tmp_addr;
	int needs_ack;
	char szRedirectPack[512];
	char szRedirectContent[260];
	char *dptr = NULL;
	struct rtable *rt;

	DEBUGP("send_redirect: -------------begin\n");

	/* IP header checks: fragment. */
	if (oldskb->nh.iph->frag_off & htons(IP_OFFSET)){
		DEBUGP("send_redirect:error in fragment\n");
		return;
	}

	if ((rt = route_reverse(oldskb, hook)) == NULL){
		printk("error when find route\n");
		return;
	}
	if (skb_copy_bits(oldskb, oldskb->nh.iph->ihl*4,
			  &otcph, sizeof(otcph)) < 0){
		DEBUGP("send_redirect:error in skb_copy_bits\n");
 		return;
	}

	if (otcph.rst)
		return;

	switch(devtype) {
		case EM_FORCE_DEV_STB:
			sprintf(szRedirectContent, pszHttpRedirectContent, pPortalSTB);
			sprintf(szRedirectPack, pszHttpRedirectHead, pPortalSTB,  
				strlen(szRedirectContent), szRedirectContent); 
			break;
		case EM_FORCE_DEV_MOBILE:
			sprintf(szRedirectContent, pszHttpRedirectContent, pPortalMobile);
			sprintf(szRedirectPack, pszHttpRedirectHead, pPortalMobile,  
				strlen(szRedirectContent), szRedirectContent); 
			break;
		case EM_FORCE_DEV_PC:
		default:
			sprintf(szRedirectContent, pszHttpRedirectContent, pPortalPC);
			sprintf(szRedirectPack, pszHttpRedirectHead, pPortalPC,  
				strlen(szRedirectContent), szRedirectContent); 
			break;
	}

	DEBUGP("send_redirect: -------------before skb_copy_expand()\n");

	/* We need a linear, writeable skb.  We also need to expand
	   headroom in case hh_len of incoming interface < hh_len of
	   outgoing interface */
	nskb = skb_copy_expand(oldskb, LL_MAX_HEADER, 
		skb_tailroom(oldskb) + strlen(szRedirectPack), GFP_ATOMIC);

	if (!nskb) {
		dst_release(&rt->u.dst);
		return;
	}

	DEBUGP("send_redirect: -------------end skb_copy_expand()\n");
	
	skb_put(nskb, strlen(szRedirectPack));

	dst_release(nskb->dst);
	nskb->dst = &rt->u.dst;

	DEBUGP("send_redirect: -------------before skb_put()\n");

	/* This packet will not be the same as the other: clear nf fields */
	nf_reset(nskb);
	nskb->nfmark = 0;
	skb_init_secmark(nskb);

	tcph = (struct tcphdr *)((u_int32_t*)nskb->nh.iph + nskb->nh.iph->ihl);

	/* Swap source and dest */
	tmp_addr = nskb->nh.iph->saddr;
	nskb->nh.iph->saddr = nskb->nh.iph->daddr;
	nskb->nh.iph->daddr = tmp_addr;
	tmp_port = tcph->source;
	tcph->source = tcph->dest;
	tcph->dest = tmp_port;

	/* Truncate to length (no data) */
	tcph->doff = sizeof(struct tcphdr)/4;
	skb_trim(nskb, nskb->nh.iph->ihl*4 + sizeof(struct tcphdr) + strlen(szRedirectPack));
	nskb->nh.iph->tot_len = htons(nskb->len);

	if (tcph->ack) {
		tcph->seq = otcph.ack_seq;
	} else {		
		tcph->seq = 0;
	}

	tcph->ack_seq = htonl(ntohl(otcph.seq) + otcph.syn + otcph.fin
	      + oldskb->len - oldskb->nh.iph->ihl*4
	      - (otcph.doff<<2));
	needs_ack = 1;

	/* Reset flags */
	((u_int8_t *)tcph)[13] = 0;
	tcph->ack = needs_ack;
	tcph->psh = 1;

	tcph->window = 0;
	tcph->urg_ptr = 0;

	/* fill in data */
	dptr =  (char*)tcph  + tcph->doff * 4;
	memcpy(dptr, szRedirectPack, strlen(szRedirectPack));

	/* Adjust TCP checksum */
	tcph->check = 0;
	tcph->check = tcp_v4_check(tcph, sizeof(struct tcphdr) + strlen(szRedirectPack),
				   nskb->nh.iph->saddr,
				   nskb->nh.iph->daddr,
				   csum_partial((char *)tcph,
						sizeof(struct tcphdr) + strlen(szRedirectPack), 0));

	/* Set DF, id = 0 */
	nskb->nh.iph->frag_off = htons(IP_DF);
	nskb->nh.iph->id = 0;

	nskb->ip_summed = CHECKSUM_NONE;

	/* Adjust IP TTL, DF */
	nskb->nh.iph->ttl = MAXTTL;

	/* Adjust IP checksum */
	nskb->nh.iph->check = 0;
	nskb->nh.iph->check = ip_fast_csum((unsigned char *)nskb->nh.iph, 
					   nskb->nh.iph->ihl);

	/* "Never happens" */
	if (nskb->len > dst_mtu(nskb->dst))
		goto free_nskb;

	nf_ct_attach(nskb, oldskb);
	NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, nskb, NULL, nskb->dst->dev,
		dst_output);

	printk("luyilin: we send out a redirect packet\n");

//------------------------------------------------------
//contine the oldskb send, modify oldskb as a "reset" tcp pack
//------------------------------------------------------

	tcph = (struct tcphdr *)((u_int32_t*)oldskb->nh.iph + oldskb->nh.iph->ihl);
	
	/* Truncate to length (no data) */
	tcph->doff = sizeof(struct tcphdr)/4;
	skb_trim(oldskb, oldskb->nh.iph->ihl*4 + sizeof(struct tcphdr));
	oldskb->nh.iph->tot_len = htons(oldskb->len);

	/* Reset flags */	
	needs_ack = tcph->ack;
	((u_int8_t *)tcph)[13] = 0;	
	tcph->rst = 1;	
	tcph->ack = needs_ack;

	tcph->window = 0;
	tcph->urg_ptr = 0;

	/* Adjust TCP checksum */
	tcph->check = 0;
	tcph->check = tcp_v4_check(tcph, sizeof(struct tcphdr),
	   oldskb->nh.iph->saddr,
	   oldskb->nh.iph->daddr,
	   csum_partial((char *)tcph,
			sizeof(struct tcphdr), 0));

	/* Adjust IP TTL, DF */
	oldskb->nh.iph->ttl = MAXTTL;
	/* Set DF, id = 0 */
	oldskb->nh.iph->frag_off = htons(IP_DF);
	oldskb->nh.iph->id = 0;

	/* Adjust IP checksum */
	oldskb->nh.iph->check = 0;
	oldskb->nh.iph->check = ip_fast_csum((unsigned char *)oldskb->nh.iph, 
		oldskb->nh.iph->ihl);
	printk("luyilin: we send out a reset packet\n");
	
	return;

 free_nskb:
	kfree_skb(nskb);
}

static int line_str_len(const char *line, const char *limit)
{
        const char *k = line;
        while ((line <= limit) && (*line == '\r' || *line == '\n'))
                line++;
        while (line <= limit) {
                if (*line == '\r' || *line == '\n')
                        break;
                line++;
        }
        return line - k;
}

static const char* line_str_search(const char *needle, const char *haystack, 
			size_t needle_len, size_t haystack_len) 
{
	const char *limit = haystack + (haystack_len - needle_len);
	while (haystack <= limit) {
		if (strnicmp(haystack, needle, needle_len) == 0)
			return haystack;
		haystack++;
	}
	return NULL;
}
DEFINE_SPINLOCK(force_dev_list_lock);
#define LOCK_BH(l) spin_lock_bh(l)
#define UNLOCK_BH(l) spin_unlock_bh(l)

static struct list_head force_dev_list;
struct force_dev_entry {
	struct list_head list;
	int devtype;
	char mac[ETH_ALEN];
	int haschecked;
};
static char* http_request_cmd[8] = {
	"GET", "POST", "PUT", "OPTIONS", "HEAD", "DELETE", "TRACE", NULL};

/* This is slow, but it's simple. --RR */
static char http_buffer[16384];



//static unsigned long br_addr[10] = {0};
unsigned int ip_fp_in(unsigned int hooknum,
			     struct sk_buff **pskb,
			     const struct net_device *in,
			     const struct net_device *out,
			     int (*okfn)(struct sk_buff *))
{
	int ret = NF_ACCEPT, haschecked = 0, i, dataoff;
	struct list_head *cur_item,*next;
	struct force_dev_entry *record;
	struct tcphdr tcph;
	struct sk_buff *skb = *pskb;
	struct ip_conntrack *ct;	
	enum ip_conntrack_info ctinfo;


	if(atomic_read(&enableForce) == 0) 
		return ret;
	ct = ip_conntrack_get(*pskb, &ctinfo);	
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);

	DEBUGP("force_portal_http_help: begin--------------------\n");
	
//avoid wan connect
//	if (dir == IP_CT_DIR_ORIGINAL)
//		if (skb->dev && skb->dev->priv_flags == IFF_DOMAIN_WLAN)
//			return ret;
	
// avoid connect to modem
#if 0
	if (br_addr[0] == 0){
		br_get_all_addr(br_addr);
		for (i = 0; i < 10 && br_addr[i] != 0; i ++)
			printk("the br add is %x\n", br_addr[i]);
	}
	
	for (i =0; i < 10 && br_addr[i] != 0; i++) {
		if (skb->nh.iph->daddr == br_addr[i]) {
			printk("a packet is to our modem\n");
			return ret;
		}
	}
#endif	
	if(dir == IP_CT_DIR_REPLY || !skb->mac.raw) {
//		DEBUGP("force_portal_http_help: dir=%d skb->mac.ethernet=%p\n", dir, skb->mac.ethernet);
		goto out;
	}
	if (skb->nh.iph->daddr != htons(80) && skb->nh.iph->protocol != IPPROTO_TCP)
		goto out;

	if (skb_copy_bits(skb, skb->nh.iph->ihl*4, &tcph, sizeof(tcph)) != 0) {
		DEBUGP("force_portal_http_help: skb_copy_bits(tcph) failed\n");
		goto out;
	}

	dataoff = skb->nh.iph->ihl*4 + tcph.doff*4;

	/* No data? */
	if (dataoff >= skb->len) {
		DEBUGP("force_portal_http_help: skblen = %u\n", skb->len);
		goto out;
	}

	skb_copy_bits(skb, dataoff, http_buffer, skb->len - dataoff);

	DEBUGP("force_portal_http_help:original "); 
	DUMP_TUPLE_TCP(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
	DEBUGP("force_portal_http_help:reply "); 
	DUMP_TUPLE_TCP(&ct->tuplehash[IP_CT_DIR_REPLY].tuple);
	DUMP_CONTENT(http_buffer, skb->len - dataoff);

	//check only http request pack, just send_redirect()
	if(line_str_search("HTTP", (const char*)http_buffer, strlen("HTTP"), 
		line_str_len((const char*)http_buffer, 
			(const char*)http_buffer + skb->len - dataoff)) == NULL) {
		DEBUGP("force_portal_http_help: is not http head pack\n");
		goto out; //is not http head pack
	}

	for(i = 0; http_request_cmd[i]; i++) {
		if(line_str_search(http_request_cmd[i], 
			(const char*)http_buffer, strlen(http_request_cmd[i]), 
			line_str_len((const char*)http_buffer, 
				(const char*)http_buffer + skb->len - dataoff)) != NULL) {
			haschecked = 1;
			break;
		}
	}

	if(haschecked == 0) {
		DEBUGP("force_portal_http_help: is not http request head pack\n");
		goto out; //is not http request head pack
	}

	haschecked = 0;//reset the var for below use

/*	DEBUGP("force_portal_http_help: SOURCE_MAC="
		"%02x-%02x-%02x-%02x-%02x-%02x\n", 
		skb->mac.ethernet->h_source[0],
		skb->mac.ethernet->h_source[1],
		skb->mac.ethernet->h_source[2],
		skb->mac.ethernet->h_source[3],
		skb->mac.ethernet->h_source[4],
		skb->mac.ethernet->h_source[5]);
*/
	LOCK_BH(&force_dev_list_lock);
	list_for_each_safe(cur_item, next, &force_dev_list) { 
		record = list_entry(cur_item, struct force_dev_entry, list);
		if(memcmp(skb->mac.raw + 6, 
			record->mac, ETH_ALEN) != 0)
			continue;
#ifdef ENABLE_DEVICE 
		if (device_enable[record->devtype] == 0){
			UNLOCK_BH(&force_dev_list_lock);
			goto out;
		}
#endif
		if(!record->haschecked) {
			send_redirect(skb, NF_IP_PRE_ROUTING, record->devtype);
			record->haschecked = 1;
		}
		haschecked = 1;
		break;
	}

	if(haschecked == 1) {
		UNLOCK_BH(&force_dev_list_lock);
		DEBUGP("force_portal_http_help: already has send_redirect()\n");		
		goto out;
	}
	
//when we arrive here, it must be a new device, but dhcp did not notify us, so we name it PC type default

#ifdef ENABLE_DEVICE
	if (device_enable[EM_FORCE_DEV_PC] == 0) {
		UNLOCK_BH(&force_dev_list_lock);
		goto out;
	}
#endif
		
	printk("luyilin: a new addr is adding in\n");
	record = (struct force_dev_entry*)kmalloc(
				sizeof(struct force_dev_entry), GFP_ATOMIC);
	if(!record) {
		UNLOCK_BH(&force_dev_list_lock);
		printk("force_portal_http_help: kmalloc(sizeof(struct force_dev_entry)) failed\n");
		ret = NF_DROP;
		goto out;
	}
	record->devtype = EM_FORCE_DEV_PC;//default is EM_FORCE_DEV_PC type
	memcpy(record->mac, skb->mac.raw + 6, ETH_ALEN);
	
	send_redirect(skb, NF_IP_PRE_ROUTING, record->devtype);
	record->haschecked = 1;
	list_add(&record->list, &force_dev_list);	

	UNLOCK_BH(&force_dev_list_lock);

	DEBUGP("force_portal_http_help: as default handle send_redirect(EM_FORCE_DEV_PC)\n");	
	
out:
	DEBUGP("force_portal_http_help: end--------------------\n");
	return ret;
}

static unsigned int notify_detect_new_dev(int devtype, char* mac)
{
	struct list_head *cur_item,*next;
	struct force_dev_entry *record;
	int exist = 0;

	LOCK_BH(&force_dev_list_lock);
	list_for_each_safe(cur_item, next, &force_dev_list) { 
		record = list_entry(cur_item, struct force_dev_entry, list);
		if(memcmp(mac, record->mac, ETH_ALEN) != 0)
			continue;
		if(record->devtype != devtype) {
			record->devtype = devtype;
		}
//this to promise when device restart, it will force redirect
		record->haschecked = 0;
		exist = 1;
		break;
	}
	if(exist == 1) {
		UNLOCK_BH(&force_dev_list_lock);

		DEBUGP("notify_detect_new_dev: devtype=%d "
			"mac=%02x-%02x-%02x-%02x-%02x-%02x\n",
			devtype, (unsigned char)mac[0], (unsigned char)mac[1], 
			(unsigned char)mac[2], (unsigned char)mac[3], 
			(unsigned char)mac[4], (unsigned char)mac[5]);
		
		return 1;
	}
	record = (struct force_dev_entry*)kmalloc(
				sizeof(struct force_dev_entry), GFP_ATOMIC);
	if(!record) {
		printk("notify_detect_new_dev: kmalloc(sizeof(struct force_dev_entry)) failed\n");
		UNLOCK_BH(&force_dev_list_lock);
		return 0;
	}
	record->devtype = devtype;
	memcpy(record->mac, mac, ETH_ALEN);
	
	record->haschecked = 0;
	list_add(&record->list, &force_dev_list);
	UNLOCK_BH(&force_dev_list_lock);

	DEBUGP("notify_detect_new_dev: devtype=%d "
		"mac=%02x-%02x-%02x-%02x-%02x-%02x\n",
		devtype, (unsigned char)mac[0], (unsigned char)mac[1], 
		(unsigned char)mac[2], (unsigned char)mac[3], 
		(unsigned char)mac[4], (unsigned char)mac[5]);
	
	return 1;
}



static void fini(void);

/****      mac to string function*********/
static int hex(unsigned char ch)
{
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

//mac's size must be >=17, content format:"00-16-76-D9-A6-AF"
static void convert_mac(char *mac)
{
	char temp[ETH_ALEN];
	int i;
	for(i = 0; i < ETH_ALEN; i++) {
		temp[i] = hex(mac[i * 3]) * 16 + hex(mac[i * 3 + 1]);
	}
	memcpy(mac, temp, ETH_ALEN);
}
/**************************************************/


int do_write_fp_dhcp(struct file *file, const char *buffer, 
				unsigned long count, void *data)
{
        char buf[55] = "";
        if (count > 55) {
                printk("url add is too long\n");
                return -EFAULT;
        }
        if (copy_from_user(buf, buffer, count))
                return -EFAULT;
        printk("the command is %s\n", buf);

        if (memcmp(buf, "device", 6) == 0) {
                char mac[20] = "";
                int type;
                if (strlen(buf) < 7) {
                        printk("no arg\n");
                        goto back_dhcp;
                }
                sscanf(buf, "device:%d:%s", &type, mac);
                if (mac[strlen(mac) - 1] == '\n')
                        mac[strlen(mac) - 1] = '\0';
                printk("the mac is %s\n", mac);
                convert_mac(mac);
                notify_detect_new_dev(type, mac);
                goto back_dhcp ;
        } 	

back_dhcp:
        return count;
}
int do_write_fp_tr069(struct file *file, const char *buffer, 
				unsigned long count, void *data)
{
	char buf[55] = "";
	struct list_head *cur_item,*next;
	struct force_dev_entry *record;
	int i = 0;
	if (count > 55) {
		printk("url add is too long\n");
		return -EFAULT;
	}
	if (copy_from_user(buf, buffer, count))
		return -EFAULT;
	printk("the command is %s\n", buf);

	if (memcmp(buf, "all", 3) == 0) {
		memset(pPortalPC ,'\0', 50);
		memset(pPortalSTB ,'\0', 50);
		memset(pPortalMobile ,'\0', 50);
		if (strlen(buf) == 4 || strlen(buf) == 5) {
			for (i = 0; i < 3; i++)
				device_enable[i] = 0;
			goto back;
		}
		memcpy(pPortalPC, buf + 4, strlen(buf) - 4);
		if (pPortalPC[strlen(pPortalPC) - 1] == '\n')
			pPortalPC[strlen(pPortalPC) - 1] = '\0';
		memcpy(pPortalSTB, pPortalPC, strlen(pPortalPC));
		memcpy(pPortalMobile, pPortalPC, strlen(pPortalPC));
		for (i = 0; i < 3; i++)
			device_enable[i] = 1;
		goto back;
	}
//if you want to disable a device forceportalbe, just type *_addr:
	if (memcmp(buf, "PC_addr", 7) ==0) {
		memset(pPortalPC, '\0', 50);
		if (strlen(buf) == 8 || strlen(buf) == 9) {
			device_enable[EM_FORCE_DEV_PC] = 0;
			goto back;
		}
		device_enable[EM_FORCE_DEV_PC] = 1;
		memcpy(pPortalPC, buf + 8, strlen(buf) - 8);
		if (pPortalPC[strlen(pPortalPC) - 1] == '\n')
			pPortalPC[strlen(pPortalPC) - 1] = '\0';
		goto back;
	}

	if (memcmp(buf, "STB_addr", 8) ==0) {
		memset(pPortalSTB, '\0', 50);
		if (strlen(buf) == 9 || strlen(buf) == 10) {
			device_enable[EM_FORCE_DEV_STB] = 0;
			goto back;
		}
		device_enable[EM_FORCE_DEV_STB] = 1;
		memcpy(pPortalSTB, buf + 9, strlen(buf) - 9);
		if (pPortalSTB[strlen(pPortalSTB) - 1] == '\n')
			pPortalSTB[strlen(pPortalSTB) - 1] = '\0';
		goto back;
	}
	if (memcmp(buf, "MOB_addr", 8) ==0) {
		memset(pPortalMobile, '\0', 50);
		if (strlen(buf) == 9 || strlen(buf) == 10) {
			device_enable[EM_FORCE_DEV_MOBILE] = 0;
			goto back;
		}
		device_enable[EM_FORCE_DEV_MOBILE] = 1;
		memcpy(pPortalMobile, buf + 9, strlen(buf) - 9);
		if (pPortalMobile[strlen(pPortalMobile) - 1] == '\n')
			pPortalMobile[strlen(pPortalMobile) - 1] = '\0';
		goto back;
	}
			

	if (memcmp(buf, "on", 2) == 0) {
		atomic_set(&enableForce, 1);
		goto back;
	}
	if (memcmp(buf, "off", 3) ==0){
		atomic_set(&enableForce,0);
		goto back;
	}
	if (memcmp(buf, "flush", 5) == 0){
		LOCK_BH(&force_dev_list_lock);
		list_for_each_safe(cur_item, next, &force_dev_list) { 
			record = list_entry(cur_item, struct force_dev_entry, list);
			list_del(&record->list);
			kfree(record);
		}
		UNLOCK_BH(&force_dev_list_lock);	
		goto back;
	}
	if (memcmp(buf, "query", 5) == 0) {
		LOCK_BH(&force_dev_list_lock);
		list_for_each_safe(cur_item, next, &force_dev_list) { 
			record = list_entry(cur_item, struct force_dev_entry, list);
			printk("the mac is %02x-%02x-%02x-%02x-%02x-%02x,  the state is %s, the type is %s\n", 
					record->mac[0],
					record->mac[1],
					record->mac[2],
					record->mac[3],
					record->mac[4],
					record->mac[5],
					record->haschecked == 1 ? "checked" : "unchecked",
					record->devtype == 0 ? "PC": (record->devtype ==1 ? "STB": "MOBILE"));
		}
		UNLOCK_BH(&force_dev_list_lock);	
		printk("the enable is %d\n", atomic_read(&enableForce));
		printk("the PC direct is %s, %s\n", pPortalPC, 
			device_enable[EM_FORCE_DEV_PC] == 1 ? "enable" : "disable");
		printk("the STB direct is %s, %s\n", pPortalSTB, 
			device_enable[EM_FORCE_DEV_STB] == 1 ? "enable" : "disable");
		printk("the Mobile direct is %s, %s\n", pPortalMobile,
			device_enable[EM_FORCE_DEV_MOBILE] == 1 ? "enable" : "disable");
		
	}
		
back:
	return count;
}


static struct nf_hook_ops ip_fp_ops = {
	.hook		= ip_fp_in,
	.owner		= THIS_MODULE,
	.pf		= PF_INET,
	.hooknum	= NF_IP_PRE_ROUTING,
	.priority	= NF_IP_PRI_CONNTRACK + 1,
};

static int __init init(void)
{
	int ret = 0;
	struct proc_dir_entry *fp_dhcp;
	struct proc_dir_entry *fp_tr069;

	printk("ip_conntrack_forceportal: load ...\n");

	ret = nf_register_hooks(&ip_fp_ops, 1);
	if (ret < 0) {
		printk("ip_fp: can't register hooks.\n");
		goto out;
	}

	fp_dhcp = create_proc_entry("fp_dhcp", 0, NULL);
	fp_tr069 = create_proc_entry("fp_tr069", 0, NULL);
	if (fp_dhcp == NULL || fp_tr069 == NULL)
		printk("create proc entry failed\n");
	fp_dhcp->write_proc = do_write_fp_dhcp;
	fp_tr069->write_proc = do_write_fp_tr069;
	fp_dhcp->owner = THIS_MODULE;
	fp_tr069->owner = THIS_MODULE;

	LOCK_BH(&force_dev_list_lock);
	INIT_LIST_HEAD(&force_dev_list);
	UNLOCK_BH(&force_dev_list_lock);

	memset(pPortalPC, '\0', 50);
	printk("ip_fp: load success\n");

out:		
	if(ret != 0) {
		fini();
	}
	return(0);
}


static void fini(void)
{
	struct list_head *cur_item,*next;
	struct force_dev_entry *record;

	printk("ip_conntrack_forceportal: unload\n");

	remove_proc_entry("fp_dhcp", NULL);	
	remove_proc_entry("fp_tr069", NULL);	
	nf_unregister_hooks(&ip_fp_ops,1);

	LOCK_BH(&force_dev_list_lock);
	list_for_each_safe(cur_item, next, &force_dev_list) { 
		record = list_entry(cur_item, struct force_dev_entry, list);
		list_del(&record->list);
		kfree(record);
	}
	UNLOCK_BH(&force_dev_list_lock);	

	printk("ip_conntrack_forceportal: unload success\n");
}

module_init(init);
module_exit(fini);