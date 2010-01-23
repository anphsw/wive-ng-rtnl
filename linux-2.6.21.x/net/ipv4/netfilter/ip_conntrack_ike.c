/* IKE connection tracking. */
#include <net/udp.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4/ip_conntrack_helper.h>
#include <linux/netfilter_ipv4/ip_conntrack_ike.h>
#define ASSERT_READ_LOCK(x)
#define ASSERT_WRITE_LOCK(x)
#include <linux/netfilter_ipv4/listhelp.h>

DEFINE_RWLOCK(ip_ike_lock);
LIST_HEAD(ike_db_list);

void ip_conntrack_udp_destroy_ike_db(struct ip_conntrack *ct)
{
	struct ip_ct_ike_db  *ike_db_entry;

	write_lock_bh(&ip_ike_lock);
	ike_db_entry = LIST_FIND(&ike_db_list, ike_find_entry_cmp_fn,
		        struct ip_ct_ike_db *, (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip), 
			(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip));

	if (ike_db_entry)
	{
		LIST_DELETE(&ike_db_list, &ike_db_entry->list);

		ike_db_entry = LIST_FIND(&ike_db_list, ike_find_entry_cmp_fn,
			        struct ip_ct_ike_db *, (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.ip), 
				(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.ip));
	}	

	write_unlock_bh(&ip_ike_lock);
}

void ip_conntrack_find_ike_db_get(u_int32_t ocookieh,u_int32_t ocookiel,
		u_int32_t rcookieh, u_int32_t rcookiel, u_int32_t src, u_int32_t dst,  
		u_int16_t sport, u_int16_t dport, u_int16_t *aliasport, u_int16_t *direction, u_int16_t *state, u_int16_t *find)
{
	struct ip_ct_ike_db  *ike_db_entry;

	write_lock_bh(&ip_ike_lock);
	ike_db_entry = LIST_FIND(&ike_db_list, ike_find_ocookie_cmp_fn,
		        struct ip_ct_ike_db *, ocookieh, 
			ocookiel);

	if (! ike_db_entry)
	{
		*find = 0;
		write_unlock_bh(&ip_ike_lock);
		return;
	}	

	*find=1;
	*state=1;

	if ((src == ike_db_entry->src) && ( dst == ike_db_entry->dst))
	{
		*direction = 0;
		write_unlock_bh(&ip_ike_lock);
		return;
	}

	if (src == ike_db_entry->dst) 
	{
		*direction = 1;
		*aliasport = ike_db_entry->aliasport;
		write_unlock_bh(&ip_ike_lock);
		return;
	}

	*state =2;

	write_unlock_bh(&ip_ike_lock);
	
	return;
}

EXPORT_SYMBOL(ike_db_list);
