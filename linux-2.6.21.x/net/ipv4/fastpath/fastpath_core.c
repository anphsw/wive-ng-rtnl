/*
Linux Kernel Hacking:
	net/core/neighbour.c				// ARP
	net/ipv4/fib_hash.c				// ROUTE
	net/ipv4/netfilter/ip_conntrack_core.c		// NAPT (PATH*2)
	net/ipv4/netfilter/ip_nat_core.c		// NAPT (PATH*2)
*/

#include "fastpath_core.h"

#define	MODULE_NAME		"Conntrack FastPath"
#define	MODULE_VERSION_FP	"v1.00"
#define PROCFS_NAME 		"ConntrackFastPath"

extern struct neigh_table arp_tbl; 
enum {
	ST_INIT =  0,
	ST_OPERATIONAL, 
	ST_ADMIN_DOWN
};

enum {
	EVT_SAR_UP = 1,
	EVT_SAR_DOWN,
	EVT_ADMIN_DOWN,
 	EVT_ADMIN_UP
};

int fp_on=1;

#if 0
#define DEBUGP_API printk
#else
#define DEBUGP_API(format, args...)
#endif

#if 0
#define DEBUGP_SYS printk
#else
#define DEBUGP_SYS(format, args...)
#endif

#define	DEBUG_PROCFILE	/* Create ProcFile for debug */

/* --- ARP Table Structures --- */
struct Arp_Table
{
	CTAILQ_HEAD(Arp_list_entry_head, Arp_List_Entry) list[ARP_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Arp_list_inuse_head, Arp_List_Entry) arp_list_inuse;
CTAILQ_HEAD(Arp_list_free_head, Arp_List_Entry) arp_list_free;

struct Arp_Table *table_arp;

/* --- Route Table Structures --- */
struct Route_Table
{
	CTAILQ_HEAD(Route_list_entry_head, Route_List_Entry) list[ROUTE_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Route_list_inuse_head, Route_List_Entry) route_list_inuse;
CTAILQ_HEAD(Route_list_free_head, Route_List_Entry) route_list_free;

struct Route_Table *table_route;

/* --- NAPT Table Structures --- */
struct Napt_List_Entry
{
	__u8 vaild;
        //enum NP_PROTOCOL protocol;
	__u16 protocol;
	ip_t intIp;
	__u32 intPort;
	ip_t extIp;
	__u32 extPort;
	ip_t remIp;
	__u32 remPort;
	enum NP_FLAGS flags;
	CTAILQ_ENTRY(Napt_List_Entry) napt_link;
	CTAILQ_ENTRY(Napt_List_Entry) tqe_link;
};

struct Napt_Table
{
	CTAILQ_HEAD(Napt_list_entry_head, Napt_List_Entry) list[NAPT_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Napt_list_inuse_head, Napt_List_Entry) napt_list_inuse;
CTAILQ_HEAD(Napt_list_free_head, Napt_List_Entry) napt_list_free;

struct Napt_Table *table_napt;

/* --- PATH Table Structures --- */
struct Path_List_Entry
{
	__u8			vaild;
	__u16			*protocol;
	ip_t		*in_sIp;
	__u32			*in_sPort;
	ip_t		*in_dIp;
	__u32			*in_dPort;
	ip_t		*out_sIp;
	__u32			*out_sPort;
	ip_t		*out_dIp;
	__u32			*out_dPort;
	__u8			*out_ifname;
	struct Arp_List_Entry	*arp_entry;		/* for Out-dMac */
	__u8			course;			/* 1:In-Bonud 2:Out-Bound */
	struct dst_entry *dst;
	__u8			type;
	CTAILQ_ENTRY(Path_List_Entry) path_link;
	CTAILQ_ENTRY(Path_List_Entry) tqe_link;
};

struct Path_Table
{
	CTAILQ_HEAD(Path_list_entry_head, Path_List_Entry) list[PATH_TABLE_LIST_MAX];
};

CTAILQ_HEAD(Path_list_inuse_head, Path_List_Entry) path_list_inuse;
CTAILQ_HEAD(Path_list_free_head, Path_List_Entry) path_list_free;

struct Path_Table *table_path;

/* ==================================================================================================== */

static __u32  FastPath_Hash_ARP_Entry(ip_t ip)
{
	return (ip % 16);
}

static __u32  
FastPath_Hash_ROUTE_Entry(ip_t ip, ip_t mask)
{
	int i;
	ip_t tmp = (ip & mask);
	
	for(i=0; i<32; i++) {
		if (tmp & 0x00000001) {
			return (tmp + (__u32)i) % ROUTE_TABLE_LIST_MAX;
		}
		tmp = tmp >> 1;
	}
	
	return 0;
}

static __u32  
FastPath_Hash_NAPT_Entry(ip_t intIp,__u32 intPort,
			ip_t extIp, __u32 extPort,
			ip_t remIp, __u32 remPort)
{
	__u32 hash;

	hash = (0xff000000 & intIp) >> 24;
	hash ^= (0x00ff0000 & intIp) >> 16;
	hash ^= (0x0000ff00 & intIp) >> 8;
	hash ^= (0x000000ff & intIp);
	hash ^= (0x0000ff00 & intPort) >> 8;
	hash ^= (0x000000ff & intPort);
	
	hash ^= (0xff000000 & extIp) >> 24;
	hash ^= (0x00ff0000 & extIp) >> 16;
	hash ^= (0x0000ff00 & extIp) >> 8;
	hash ^= (0x000000ff & extIp);
	hash ^= (0x0000ff00 & extPort) >> 8;
	hash ^= (0x000000ff & extPort);

	hash ^= (0xff000000 & remIp) >> 24;
	hash ^= (0x00ff0000 & remIp) >> 16;
	hash ^= (0x0000ff00 & remIp) >> 8;
	hash ^= (0x000000ff & remIp);
	hash ^= (0x0000ff00 & remPort) >> 8;
	hash ^= (0x000000ff & remPort);

	//return 0x000003ff & (hash ^ (hash >> 12));
	return (NAPT_TABLE_LIST_MAX-1) & (hash ^ (hash >> 12));
}

inline static __u32 FastPath_Hash_PATH_Entry(ip_t sip, __u32 sport, ip_t dip, __u32 dport, __u16 proto)
{
	register __u32 hash;
	
	hash = ((sip>>16)^sip);
	hash ^= ((dip>>16)^dip);
	hash ^= sport;
	hash ^= dport;
	hash ^= proto;
	//return 0x000003ff & (hash ^ (hash >> 12));
	return (PATH_TABLE_LIST_MAX-1) & (hash ^ (hash >> 12));
}


/* ==================================================================================================== */

enum LR_RESULT  
fastpath_addArp(ip_t ip,
		ether_t* mac,
		enum ARP_FLAGS flags)
{
	__u32 hash = FastPath_Hash_ARP_Entry(ip);
	struct Arp_List_Entry *ep;
	
	DEBUGP_API("addArp: ip=0x%08X mac=%02X:%02X:%02X:%02X:%02X:%02X flags=0x%08X Hash=%u \n", ip, MAC2STR(*mac), flags, hash);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ip) {
			DEBUGP_SYS("addArp: ERROR - arp(ip=0x%08X) already exist! \n", ip);
			return LR_EXIST;
		}
	}
	
	/* Create */
	if(!CTAILQ_EMPTY(&arp_list_free)) {
		struct Arp_List_Entry *entry_arp;
		entry_arp = CTAILQ_FIRST(&arp_list_free);
		entry_arp->ip = ip;
		entry_arp->mac = *mac;
		entry_arp->flags = flags;
		entry_arp->vaild = 0xff;
		CTAILQ_REMOVE(&arp_list_free, entry_arp, tqe_link);
		CTAILQ_INSERT_TAIL(&arp_list_inuse, entry_arp, tqe_link);
		CTAILQ_INSERT_TAIL(&table_arp->list[hash], entry_arp, arp_link);
	} else {
		DEBUGP_SYS("addArp: ERROR - arp_list_free is empty! \n");
		return LR_FAILED;
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT  
fastpath_modifyArp(ip_t ip,
		ether_t* mac,
		enum ARP_FLAGS flags)
{
	__u32 hash = FastPath_Hash_ARP_Entry(ip);
	struct Arp_List_Entry *ep;
	
	DEBUGP_API("modifyArp: ip=0x%08X mac=%02X:%02X:%02X:%02X:%02X:%02X flags=0x%08X \n", ip, MAC2STR(*mac), flags);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ip) {
			ep->mac = *mac;
			ep->flags = flags;
			
			return LR_SUCCESS;
		}
	}
	
	return LR_SUCCESS;
}

/*
	delArp() - Delete an entry of Arp Table.
*/
enum LR_RESULT  
fastpath_delArp(ip_t ip)
{
	__u32 hash = FastPath_Hash_ARP_Entry(ip);
	struct Arp_List_Entry *ep;
	
	DEBUGP_API("delArp: ip=0x%08X \n", ip);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ip) {
			ep->vaild = 0x00;
			CTAILQ_REMOVE(&table_arp->list[hash], ep, arp_link);
			CTAILQ_REMOVE(&arp_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&arp_list_free, ep, tqe_link);
			
			return LR_SUCCESS;
		}
	}
	
	return LR_NONEXIST;
}

u32 LANsub[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u32 LANmask[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
u32 routeIndex = 0;

__u32 ExistInLAN(ip_t ip, ip_t mask){
	__u32 i;
	if(ip){
		for( i=0; i<routeIndex&& routeIndex<=8; i++ ) 
			if( LANsub[i] == ip && LANmask[i] == mask ) 		
				return 1;	//found it, return 1
	}
	return 0; //doesnt exist, return 0
}


enum LR_RESULT  
fastpath_addRoute(ip_t ip,
		ip_t mask,
		ip_t gateway,
		__u8* ifname,
		enum RT_FLAGS flags,
		int type)
{
	__u32 hash = FastPath_Hash_ROUTE_Entry(ip, mask);
	
	/* For fixing the bug that system will be hang if keep changing secondary IP&netmask . */
        // Just skip the addition if node already existed in array 
	struct Route_List_Entry *ep;		
	/* Looking for the matched ip & netmask node in list */
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) {
		if(ep->ip==ip && ep->mask == mask)			
			return LR_SUCCESS;	
        }
	DEBUGP_API("addRoute: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%s flags=0x%08X Hash=%u type=%d\n", 
		ip, mask, gateway, ifname, flags, hash, type);
	
	if(!CTAILQ_EMPTY(&route_list_free)) {
		struct Route_List_Entry *entry_route;
		entry_route = CTAILQ_FIRST(&route_list_free);
		entry_route->ip = ip;
		entry_route->mask = mask;
		entry_route->gateway = gateway;
		memcpy(&entry_route->ifname, ifname, IFNAME_LEN_MAX - 1);
		entry_route->flags = flags;
		entry_route->vaild = 0xff;
		CTAILQ_REMOVE(&route_list_free, entry_route, tqe_link);
		CTAILQ_INSERT_TAIL(&route_list_inuse, entry_route, tqe_link);
		CTAILQ_INSERT_TAIL(&table_route->list[hash], entry_route, route_link);
		//for multi-subnet
		if( type == RTN_UNICAST && strstr(ifname, "br") ) {
                     	/* For fixing the bug that system will be hang if keep changing secondary IP&netmask . */
 			//Add it if ip&mask we dont know
			if(ExistInLAN(ip, mask)==0 && routeIndex < 8){ //add restriction for routeIndex to avoid memory overlay
			LANsub[routeIndex] = ip & mask;
			LANmask[routeIndex] = mask;
			routeIndex++;
		}
		}
	} else {
		DEBUGP_SYS("addRoute: ERROR - Route_list_free is empty! \n");
		
		return LR_FAILED;
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT  
fastpath_modifyRoute(ip_t ip,
		ip_t mask,
		ip_t gateway,
		__u8* ifname,
		enum RT_FLAGS flags,
		int type)
{
	__u32 hash = FastPath_Hash_ROUTE_Entry(ip, mask);
	struct Route_List_Entry *ep;
	
	DEBUGP_API("modifyRoute: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%s flags=0x%08X \n", 
		ip, mask, gateway, ifname, flags);
		
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_route->list[hash], route_link) {
		if (ep->ip == ip && ep->mask == mask) {
			ep->gateway = gateway;
			memcpy(&ep->ifname, ifname, IFNAME_LEN_MAX - 1);
			ep->flags = flags;
			CTAILQ_REMOVE(&table_route->list[hash], ep, route_link);
			CTAILQ_REMOVE(&route_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&route_list_free, ep, tqe_link);
			return LR_SUCCESS;
		}
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT  
fastpath_delRoute(ip_t ip, ip_t mask)
{
	__u32 hash = FastPath_Hash_ROUTE_Entry(ip, mask);
	struct Route_List_Entry *ep;
	int i;
	DEBUGP_API("delRoute: ip=0x%08X mask=0x%08X \n", ip, mask);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_route->list[hash], route_link) {
		if (ep->ip == ip && ep->mask == mask) {
			ep->vaild = 0x00;
			CTAILQ_REMOVE(&table_route->list[hash], ep, route_link);
			CTAILQ_REMOVE(&route_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&route_list_free, ep, tqe_link);
			//for multi-subnet
			for( i=0; i<routeIndex&& routeIndex<=8; i++ ) {
				if( (LANsub[i] == (ip&mask))  &&  (LANmask[i] == mask) ) {
					LANsub[i] = 0;
					LANmask[i] = 0;
					routeIndex--;
					break;
				}
			}
			for( ; i<routeIndex&&routeIndex<=8; i++ ) {
				LANsub[i] = LANsub[i+1];
				LANmask[i] = LANmask[i+1];
			}									
			return LR_SUCCESS;
		}
	}
	
	return LR_NONEXIST;
}

enum LR_RESULT  
fastpath_addSession(__u8* ifname,
		enum SE_TYPE seType,
		__u32 sessionId,
		enum SE_FLAGS flags )
{
	return LR_SUCCESS;
}

enum LR_RESULT  
fastpath_delSession(__u8* ifname)
{
	return LR_SUCCESS;
}
struct Arp_List_Entry *FastPath_Find_ARP_Entry(ip_t ipaddr)
{
	struct Arp_List_Entry *ep;
	__u32 hash = FastPath_Hash_ARP_Entry(ipaddr);
	
	CTAILQ_FOREACH(ep, &table_arp->list[hash], arp_link) {
		if (ep->ip == ipaddr) {
			return ep;			
		}
	}
	return NULL;
}

struct Route_List_Entry *
rtl867x_lookupRoute(ip_t ip)		
{
	struct Route_List_Entry *ep,*default_route=NULL;
		
	/* Lookup */
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) 
	{
			if ((ep->vaild==0xff)&&((ip & ep->mask) == (ep->ip & ep->mask)))
			{
				if(ep->mask!=0)
				{
					return ep;
				}
				else
				{
					default_route=ep;
				}
			}	

	}
	
	return default_route;
}

enum LR_RESULT   
fastpath_addNaptConnection(struct ip_conntrack_tuple ori_tuple,
		struct ip_conntrack_tuple reply_tuple, enum NP_FLAGS flags)
{
	__u32 hash;
	struct Napt_List_Entry *ep;
	__u8 *proto;
	__u16 protocol;
	struct ip_conntrack_tuple tpdir1, tpdir2;
	ip_t intIp, extIp, remIp;
	__u32 intPort, extPort, remPort;
	int i;

	tpdir1.src.ip = 0;
	tpdir2.src.ip = 0;
	tpdir2.dst.ip = 0;
	
	protocol = ori_tuple.dst.protonum;	

	if( protocol == IPPROTO_TCP ) {
		proto = "TCP";
	}
	else if( protocol == IPPROTO_UDP ) {
		proto = "UDP";
	}
	else {
		proto = "unknow";
	}

	//for multi-subnet
	for(i=0; i< routeIndex; i++) {
		if ( (ori_tuple.src.ip & LANmask[i]) == LANsub[i] ) {
			tpdir1 = ori_tuple;
			tpdir2 = reply_tuple;
			break;
		}
		else {
			tpdir1 = reply_tuple;
			tpdir2 = ori_tuple;
		}
	}	

	intIp = tpdir1.src.ip;
	intPort = ntohs(tpdir1.src.u.all);
	extIp = tpdir2.dst.ip;
	extPort = ntohs(tpdir2.dst.u.all);
	remIp = tpdir2.src.ip;
	remPort = ntohs(tpdir2.src.u.all);
	
	hash = FastPath_Hash_NAPT_Entry(intIp, intPort, extIp, extPort, remIp, remPort);
	
	DEBUGP_API("addNaptConnection: P=%s int=%u.%u.%u.%u:%u ext=%u.%u.%u.%u:%u rem=%u.%u.%u.%u:%u F=%d (H=%u, Ha=%u, Hb=%u)\n", 
		proto, NIPQUAD(intIp), intPort, NIPQUAD(extIp), extPort, NIPQUAD(remIp), remPort, flags, hash,
		FastPath_Hash_PATH_Entry(intIp, intPort, remIp, remPort, protocol), FastPath_Hash_PATH_Entry(remIp, remPort, extIp, extPort, protocol));

	/* Lookup */
	CTAILQ_FOREACH(ep, &table_napt->list[hash], napt_link) {
		if ((ep->protocol == protocol) &&
			(ep->intIp == intIp) &&
			(ep->intPort == intPort) &&
			(ep->extIp == extIp) &&
			(ep->extPort == extPort) &&
			(ep->remIp == remIp) &&
			(ep->remPort == remPort)) {
			DEBUGP_SYS("addNaptConnection: ERROR - the entry already exist! \n");
			
			return LR_SUCCESS;
		}
	}
	
	if(!CTAILQ_EMPTY(&napt_list_free) && !CTAILQ_EMPTY(&path_list_free)) {
		struct Napt_List_Entry *entry_napt;
		entry_napt = CTAILQ_FIRST(&napt_list_free);
		entry_napt->protocol = protocol;
		entry_napt->intIp = intIp;
		entry_napt->intPort = intPort;
		entry_napt->extIp = extIp;
		entry_napt->extPort = extPort;
		entry_napt->remIp = remIp;
		entry_napt->remPort = remPort;
		entry_napt->flags = flags;
		entry_napt->vaild = 0xff;
		CTAILQ_REMOVE(&napt_list_free, entry_napt, tqe_link);
		CTAILQ_INSERT_TAIL(&napt_list_inuse, entry_napt, tqe_link);
		CTAILQ_INSERT_TAIL(&table_napt->list[hash], entry_napt, napt_link);
		
		/* add Path Table Entry */
		if (1) {
			__u32	hash;
			struct Path_List_Entry *entry_path;
		//downstream
			/* course = 2 (Inbound) */
			hash = FastPath_Hash_PATH_Entry(remIp, remPort, extIp, extPort, protocol);
			entry_path = CTAILQ_FIRST(&path_list_free);
			entry_path->protocol	= &entry_napt->protocol;
			entry_path->in_sIp		= &entry_napt->remIp;
			entry_path->in_sPort	= &entry_napt->remPort;
			entry_path->in_dIp		= &entry_napt->extIp;
			entry_path->in_dPort	= &entry_napt->extPort;
			entry_path->out_sIp		= &entry_napt->remIp;
			entry_path->out_sPort	= &entry_napt->remPort;
			entry_path->out_dIp		= &entry_napt->intIp;
			entry_path->out_dPort	= &entry_napt->intPort;
			entry_path->out_ifname	= FastPath_Route(*entry_path->out_dIp);
			entry_path->arp_entry	= NULL;
			entry_path->course		= 2;
			entry_path->vaild		= 0xff;
			entry_path->dst			= NULL;
			entry_path->type		= 0;	/* Init: Normal (Only Routing) */
			if (*entry_path->in_dIp != *entry_path->out_dIp) {
				entry_path->type |= 4;	/* DNAT */
			}
			if (*entry_path->in_dPort != *entry_path->out_dPort) {
				entry_path->type |= 8;	/* DNPT */
			}
			CTAILQ_REMOVE(&path_list_free, entry_path, tqe_link);
			CTAILQ_INSERT_TAIL(&path_list_inuse, entry_path, tqe_link);
			CTAILQ_INSERT_TAIL(&table_path->list[hash], entry_path, path_link);
			
		}
	} else {
		DEBUGP_SYS("addNaptConnection: ERROR - Napt_list_free is empty! \n");
		
		return LR_FAILED;
	}
	
	return LR_SUCCESS;
}

enum LR_RESULT  
fastpath_delNaptConnection (struct ip_conntrack_tuple ori_tuple, struct ip_conntrack_tuple reply_tuple)
{
	__u32 hash;
	struct Napt_List_Entry *ep;

	__u8 *proto;
	__u16 protocol;
	struct ip_conntrack_tuple tpdir1, tpdir2;
	ip_t intIp, extIp, remIp;
	__u32 intPort, extPort, remPort;
	int i;
	protocol = ori_tuple.dst.protonum;

	tpdir1.src.ip = 0;
	tpdir2.src.ip = 0;
	tpdir2.dst.ip = 0;
	
	if( protocol == IPPROTO_TCP ) {
		proto = "TCP";
	}
	else if( protocol == IPPROTO_UDP ) {
		proto = "UDP";
	}
	else {
		proto = "unknow";
	}

	//for multi-subnet
	for(i=0; i< routeIndex; i++) {
		if ( (ori_tuple.src.ip & LANmask[i]) == LANsub[i] ) {
			tpdir1 = ori_tuple;
			tpdir2 = reply_tuple;
			break;
		}
		else {
			tpdir1 = reply_tuple;
			tpdir2 = ori_tuple;
		}
	}	
	

	intIp = tpdir1.src.ip;
	intPort = ntohs(tpdir1.src.u.all);
	extIp = tpdir2.dst.ip;
	extPort = ntohs(tpdir2.dst.u.all);
	remIp = tpdir2.src.ip;
	remPort = ntohs(tpdir2.src.u.all);
	
	hash = FastPath_Hash_NAPT_Entry(intIp, intPort, extIp, extPort, remIp, remPort);
	
	DEBUGP_API("delNaptConnection: P=%s int=%u.%u.%u.%u:%u ext=%u.%u.%u.%u:%u rem=%u.%u.%u.%u:%u \n", 
		proto, NIPQUAD(intIp), intPort, NIPQUAD(extIp), extPort, NIPQUAD(remIp), remPort);
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &table_napt->list[hash], napt_link) {
		if ((ep->protocol == protocol) &&
			(ep->intIp == intIp) &&
			(ep->intPort == intPort) &&
			(ep->extIp == extIp) &&
			(ep->extPort == extPort) &&
			(ep->remIp == remIp) &&
			(ep->remPort == remPort)) {
			ep->vaild = 0x00;
			CTAILQ_REMOVE(&table_napt->list[hash], ep, napt_link);
			CTAILQ_REMOVE(&napt_list_inuse, ep, tqe_link);
			CTAILQ_INSERT_TAIL(&napt_list_free, ep, tqe_link);
			
			/* del Path Table Entry */
			if (1) {
				__u32	hash;
				struct Path_List_Entry *entry_path;
		//downstream
				/* course = 2 (Inbound) */
				hash = FastPath_Hash_PATH_Entry(remIp, remPort, extIp, extPort, protocol);
				CTAILQ_FOREACH(entry_path, &table_path->list[hash], path_link) {
					if ((entry_path->protocol == &ep->protocol) && (entry_path->course == 2)) {
						entry_path->vaild = 0x00;
						//fix dst destroyed before fastpath entry releases it
						if( entry_path->dst ) {
							dst_release(entry_path->dst);							
						}
						CTAILQ_REMOVE(&table_path->list[hash], entry_path, path_link);
						CTAILQ_REMOVE(&path_list_inuse, entry_path, tqe_link);
						CTAILQ_INSERT_TAIL(&path_list_free, entry_path, tqe_link);
						break;
					}
				}
			}
					
			return LR_SUCCESS;
		}
	}
	
	return LR_NONEXIST;
}

/* ==================================================================================================== */
__u8 *
FastPath_Route(ip_t dIp)
{
	__u8 *ifname = NULL;
	__u32 mask_max = 0x0;
	struct Route_List_Entry *ep;
	
	/* Lookup */
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) {
		if ((ep->mask >= mask_max) && ((dIp & ep->mask) == ep->ip)) {
			ifname = &ep->ifname[0];
			mask_max = ep->mask;
		}
	}
	
	return ifname;
}

#ifdef	DEBUG_PROCFILE
static int  fastpath_table_arp(char *buffer, char **start, off_t offset, int length)
{
	struct Arp_List_Entry *ep;
	int len=0;
	
	CTAILQ_FOREACH(ep, &arp_list_inuse, tqe_link) {
		len += sprintf(buffer + len, "~Arp: ip=0x%08X mac=%02X:%02X:%02X:%02X:%02X:%02X flags=0x%08X \n", ep->ip, MAC2STR(ep->mac), ep->flags);
	}
	
	return len;
}

static int  fastpath_table_route(char *buffer, char **start, off_t offset, int length)
{
	struct Route_List_Entry *ep;
	int len=0;
	
	CTAILQ_FOREACH(ep, &route_list_inuse, tqe_link) {
		len += sprintf(buffer + len, "~Route: ip=0x%08X mask=0x%08X gateway=0x%08X ifname=%-5s flags=0x%08X \n", 
			ep->ip, ep->mask, ep->gateway, ep->ifname, ep->flags);
	}
	
	return len;
}

static int  fastpath_table_napt(char *buffer, char **start, off_t offset, int length)
{
	struct Napt_List_Entry *ep;
	unsigned int len=0, newlen=0;
	__u8 *proto;
	off_t upto = 0;	
	CTAILQ_FOREACH(ep, &napt_list_inuse, tqe_link) {
		if( upto++ < offset ) 
			continue;
		if( ep->protocol  == IPPROTO_TCP ) {
			proto = "TCP";
		}
		else if( ep->protocol  == IPPROTO_UDP ) {
			proto = "UDP";
		}
		else {
			proto = "unknow";
		}
		newlen = sprintf(buffer + len, "~Napt: [%s] int=0x%08X:%-5u ext=0x%08X:%-5u rem=0x%08X:%-5u flags=0x%08X \n", 
			proto,
			ep->intIp, ep->intPort, ep->extIp, ep->extPort, ep->remIp, ep->remPort,
			ep->flags);
		if ( (len + newlen) > length) {
			goto finished;			
		}
		else {
			len += newlen;
		}
	}
finished:
	/* `start' hack - see fs/proc/generic.c line ~165 */
	*start = (char *)((unsigned int)upto - offset);	
	return len;
}

static int  fastpath_table_path(char *buffer, char **start, off_t offset, int length)
{
	struct Path_List_Entry *ep;
	unsigned int len=0, newlen=0;
	__u8 *proto;
	off_t upto = 0;	
	CTAILQ_FOREACH(ep, &path_list_inuse, tqe_link) {
		if( upto++ < offset ) 
			continue;
		if( *ep->protocol  == IPPROTO_TCP ) {
			proto = "TCP";
		}
		else if( *ep->protocol  == IPPROTO_UDP ) {
			proto = "UDP";
		}
		else {
			proto = "unknow";
		}
		newlen = sprintf(buffer + len, "~Path: [%s] in-S=0x%08X:%-5u in-D=0x%08X:%-5u out-S=0x%08X:%-5u out-D=0x%08X:%-5u out-ifname=%-5s <%u> {%d}\n", 
			proto,
			*ep->in_sIp, *ep->in_sPort, *ep->in_dIp, *ep->in_dPort,
			*ep->out_sIp, *ep->out_sPort, *ep->out_dIp, *ep->out_dPort,
			ep->out_ifname, ep->course, ep->type);
		if ( (len + newlen) > length) {
			goto finished;			
		}
		else {
			len += newlen;
		}
	}
finished:
	/* `start' hack - see fs/proc/generic.c line ~165 */
	*start = (char *)((unsigned int)upto - offset);		
	return len;
}

static int  fastpath_hash_path(char *buffer, char **start, off_t offset, int length)
{
	int i, len=0;
	
	for (i=0; i<PATH_TABLE_LIST_MAX; i++) {
		len += sprintf(buffer + len, "%5d ", CTAILQ_TOTAL(&table_path->list[i]));
		if (i%12 == 11) len += sprintf(buffer + len, "\n");
	}
	len += sprintf(buffer + len, "\n");	
	
	return len;
}

#endif	/* DEBUG_PROCFILE */

static struct proc_dir_entry *FP_Proc_File;
#include <asm/uaccess.h>
int fp_proc_read(struct file *file, const char *buffer,
                      unsigned long count, void *data)
{
	//extern void Route_Iterate();

	if(fp_on==1)
		printk("fastpath ON!\n");
	if(fp_on==0)
		printk("fastpath OFF!\n");
    return -1;
}

int fp_proc_write(struct file *file, const char *buffer,
                      unsigned long count, void *data)
{
	char proc_buffer[count];
	
	/* write data to the buffer */
	memset(proc_buffer, 0, sizeof(proc_buffer));
	if ( copy_from_user(proc_buffer, buffer, count) ) {
		return -EFAULT;
	}

	switch(proc_buffer[0]) {
	case '0': fp_on = 0;break;
	case '1':	fp_on = 1;break;
	default:
		printk("Error setting!\n");
	}

    return -1;
}

static int  fastpath_memory_init(void) 
{
	int i;	
	/* Arp-Table Init */
	table_arp = (struct Arp_Table *)kmalloc(sizeof(struct Arp_Table), GFP_ATOMIC);
	if (table_arp == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Arp Table) \n");
		return -1;
	}
	CTAILQ_INIT(&arp_list_inuse);
	CTAILQ_INIT(&arp_list_free);
	for (i=0; i<ARP_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_arp->list[i]);
	}
	/* Arp-List Init */
	for (i=0; i<ARP_TABLE_ENTRY_MAX; i++) {
		struct Arp_List_Entry *entry_arp = (struct Arp_List_Entry *)kmalloc(sizeof(struct Arp_List_Entry), GFP_ATOMIC);
		if (entry_arp == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Arp Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&arp_list_free, entry_arp, tqe_link);
	}
	
	/* Route-Table Init */
	table_route = (struct Route_Table *)kmalloc(sizeof(struct Route_Table), GFP_ATOMIC);
	if (table_route == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Route Table) \n");
		return -1;
	}
	CTAILQ_INIT(&route_list_inuse);
	CTAILQ_INIT(&route_list_free);
	for (i=0; i<ROUTE_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_route->list[i]);
	}
	/* Route-List Init */
	for (i=0; i<ROUTE_TABLE_ENTRY_MAX; i++) {
		struct Route_List_Entry *entry_route = (struct Route_List_Entry *)kmalloc(sizeof(struct Route_List_Entry), GFP_ATOMIC);
		if (entry_route == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Route Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&route_list_free, entry_route, tqe_link);
	}
	
	/* Napt-Table Init */
	table_napt = (struct Napt_Table *)kmalloc(sizeof(struct Napt_Table), GFP_ATOMIC);
	if (table_napt == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Napt Table) \n");
		return -1;
	}
	CTAILQ_INIT(&napt_list_inuse);
	CTAILQ_INIT(&napt_list_free);
	for (i=0; i<NAPT_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_napt->list[i]);
	}
	/* Napt-List Init */
	for (i=0; i<NAPT_TABLE_ENTRY_MAX; i++) {
		struct Napt_List_Entry *entry_napt = (struct Napt_List_Entry *)kmalloc(sizeof(struct Napt_List_Entry), GFP_ATOMIC);
		if (entry_napt == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Napt Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&napt_list_free, entry_napt, tqe_link);
	}
	
	/* Path-Table Init */
	table_path = (struct Path_Table *)kmalloc(sizeof(struct Path_Table), GFP_ATOMIC);
	if (table_path == NULL) {
		DEBUGP_SYS("MALLOC Failed! (Path Table) \n");
		return -1;
	}
	CTAILQ_INIT(&path_list_inuse);
	CTAILQ_INIT(&path_list_free);
	for (i=0; i<PATH_TABLE_LIST_MAX; i++) {
		CTAILQ_INIT(&table_path->list[i]);
	}
	/* Path-List Init */
	for (i=0; i<PATH_TABLE_ENTRY_MAX; i++) {
		struct Path_List_Entry *entry_path = (struct Path_List_Entry *)kmalloc(sizeof(struct Path_List_Entry), GFP_ATOMIC);
		if (entry_path == NULL) {
			DEBUGP_SYS("MALLOC Failed! (Path Table Entry) \n");
			return -2;
		}
		CTAILQ_INSERT_TAIL(&path_list_free, entry_path, tqe_link);
	}

	return 0;
}

static int __init fastpath_init(void)
{
#ifdef	DEBUG_PROCFILE
	/* proc file for debug */
	proc_net_create("fp_arp", 0, fastpath_table_arp);
	proc_net_create("fp_route", 0, fastpath_table_route);
	proc_net_create("fp_napt", 0, fastpath_table_napt);
	proc_net_create("fp_path", 0, fastpath_table_path);
	proc_net_create("fp_hash_path", 0, fastpath_hash_path);
#endif	/* DEBUG_PROCFILE */
	
	
	
	printk("%s %s\n",MODULE_NAME, MODULE_VERSION_FP);
	
	//create proc
	FP_Proc_File= create_proc_entry(PROCFS_NAME, 0644, NULL);
	if (FP_Proc_File == NULL) {
		remove_proc_entry(PROCFS_NAME, &proc_root);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
			PROCFS_NAME);
		return -ENOMEM;
	}

	FP_Proc_File->read_proc  = (read_proc_t *)fp_proc_read;
	FP_Proc_File->write_proc  = (write_proc_t *)fp_proc_write;
	FP_Proc_File->owner 	  = THIS_MODULE;
	FP_Proc_File->mode 	  = S_IFREG | S_IRUGO;
	FP_Proc_File->uid 	  = 0;
	FP_Proc_File->gid 	  = 0;
	FP_Proc_File->size 	  = 37;

	printk(KERN_INFO "/proc/%s created\n", PROCFS_NAME);
	fastpath_memory_init();
    return 0;
}

static void __exit fastpath_exit(void)
{
	printk("%s %s removed!\n", MODULE_NAME, MODULE_VERSION_FP);
}

module_init(fastpath_init);
module_exit(fastpath_exit);
MODULE_LICENSE("GPL");
