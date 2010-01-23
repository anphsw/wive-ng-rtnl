
#ifndef	__NF_STUN
#define	__NF_STUN

	#include <linux/list.h>
	#include <linux/module.h>
	#include <linux/in.h>
	#include <linux/spinlock.h>
	#include <linux/netfilter_ipv4/ipt_STUN_common.h>
	#include <linux/netfilter_ipv4/ip_conntrack_tuple.h>
	#include <linux/netfilter_ipv4/ip_nat_helper.h>
	#include <linux/netfilter_ipv4/ip_nat.h>
	#include <linux/version.h>
	
	#define CLR_NAT_SYMMETRIC(flag_p)				clear_bit(3, flag_p)
	#define CLR_NAT_FULL_CONE(flag_p)				clear_bit(4, flag_p)
	#define CLR_NAT_RESTRICTED_CONE(flag_p)			clear_bit(5, flag_p)
	#define CLR_NAT_PORT_RESTRICTED_CONE(flag_p)	clear_bit(6, flag_p)
	
	#define STUN_ARRAY_SIZE		128
	#define RG_IPADDR_MAX		4
    #define STUN_TIMER_DELAY    120*HZ  /*sec*/  
    #define STUN_TIMER_STALE    300*HZ
	
	#define	__UPTIME_SEC__	(((jiffies-UPTIME)/HZ)%60)
	#define	__UPTIME_MIN__	((((jiffies-UPTIME)/HZ)/60)%60)
	#define	__UPTIME_HUR__	((((jiffies-UPTIME)/HZ)/60)/60)
	#define STUN_UPTIME		printk("STUN uptime:: %ld:%ld:%ld " \
		, __UPTIME_HUR__, __UPTIME_MIN__, __UPTIME_SEC__)
	#define STUN_ERROR		printk("::Error at %s- #%d- %s\n" \
		, __FILE__, __LINE__, __func__)
	#define STUN_TRIGGER_I	printk("Trigger ingress at %s- #%d- %s\n" \
		, __FILE__, __LINE__, __func__)
	#define STUN_TRIGGER_O	printk("Trigger egress at %s- #%d- %s\n" \
		, __FILE__, __LINE__, __func__)
	
	typedef	struct	session_flow {
		uint32_t	ip;
		uint16_t	port;
	}SESSION_FLOW;
	
	typedef struct	stun_array {
		struct list_head	list;
		uint16_t	index;
		uint8_t		unused:5, type:3;
		#define	STUN_REF_MAX_COUNT	15
		uint8_t		count_ref:4, count_ref_in:4;
		uint16_t	protonum;
		SESSION_FLOW	src, dst, map;
		unsigned long	timestamp;
	}STUN_NAT;
	
	typedef struct	mapping_table {
		struct list_head	list;
		uint16_t		index;
		uint16_t		protonum;
		SESSION_FLOW	m;
		uint16_t		counting;
	}STUN_MTABLE;
	
	struct stun_nat_helper {
		//int			*counting;		// Why it makes sys panic??
		spinlock_t	*stun_lock;
		rwlock_t	*stun_rwlock;
		int		(*stun_array_add)(SESSION_FLOW *, 
								SESSION_FLOW *, 
								uint16_t *, 
								SESSION_FLOW *, 
								STUN);
		void	(*stun_inc_refcount)(SESSION_FLOW *, 
									SESSION_FLOW *, 
									SESSION_FLOW *, 
									uint16_t, 
									STUN_DIR);
		int		(*stun_check_packet)(SESSION_FLOW *, 
									SESSION_FLOW *, 
									uint16_t);
		/*int		(*mangle_packet)(struct sk_buff **, 
								uint16_t, 
								uint16_t);*/
		int		(*stun_check_port)(uint16_t, 
								uint16_t);
		void	(*stun_array_show)(int);
		unsigned int	(*stun_manip_packet)(struct ip_conntrack *, 
											unsigned int, 
											struct sk_buff **);
		STUN_NAT	(*stun_find_stunnat)(SESSION_FLOW *, 
										SESSION_FLOW *, 
										SESSION_FLOW *, 
										uint16_t, 
										int);
		void	(*stun_destory)(SESSION_FLOW *, 
								SESSION_FLOW *, 
								SESSION_FLOW *, 
								uint16_t);
        int     (*stun_check_ip)(uint32_t); 
        int     (*stun_verdict_dmz)(uint16_t, 
                                    uint16_t, 
                                    uint32_t, 
                                    uint16_t);
	};
	
	/* func declarion */
	unsigned int	ip_nat_setup_info_for_stun(struct ip_conntrack *conntrack, 
				      						const struct ip_nat_range *, 
				      						unsigned int, 
				      						STUN);
	/* extern variable and funcs */
	extern struct stun_nat_helper	*stun_helper;
	//extern unsigned long 			UPTIME;
	extern int	register_stun_nat(struct stun_nat_helper *, 
								int *);
	extern void	unregister_stun_nat(struct stun_nat_helper *);
#endif //__NF_STUN
