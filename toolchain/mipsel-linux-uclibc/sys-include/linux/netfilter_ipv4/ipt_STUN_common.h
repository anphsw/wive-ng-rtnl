
#ifndef	__NF_STUN_COMMON
#define	__NF_STUN_COMMON
	
	#define NAT_LIST_DEBUG	0		/* Debugging in kernel space, netfilter */
	#define STUN_DEBUG		0		/* Debugging in userspace, iptables */
	#ifndef	STUN_PROC_DEBUG
		#define	STUN_PROC_DEBUG		/* Show the info of stun_array into /proc */
	#endif
	
	/* used in flags of struct ip_nat_range */
	#define	IP_NAT_SYMMETRIC			0x00000008
	#define	IP_NAT_FULL_CONE			0x00000010
	#define	IP_NAT_RESTRICTED_CONE		0x00000020
	#define	IP_NAT_PORT_RESTRICTED_CONE	0x00000040
	
	/* Recording the STUN type in STUN_NAT */
	#define IPS_STUN_PORT_RESTRICTED_CONE	0x1
    #define IPS_STUN_RESTRICTED_CONE		0x2
    #define IPS_STUN_FULL_CONE				0x3
    #define IPS_STUN_SYMMETRIC				0x4
	
	/* copy from skdrv1st.h */
	#define int8_t		char
	#define int16_t		short
	#define int32_t		long
	#define int64_t		long long
	#define uint8_t		u_char
	#define uint16_t	u_short
	#define uint32_t	u_long
	#define uint64_t	unsigned long long
	
	/* type of STUN */
	typedef enum	stun {
		__NONE, 
		__PORT_RESTRICTED_CONE, 
		__RESTRICTED_CONE, 
		__FULL_CONE, 
		__SYMMETRIC_NAT, 
		__ERROR
	}STUN;
	
	/* direction of STUN */
	typedef enum	stun_dir {
		__STUN_DIR_IN, 
		__STUN_DIR_OUT
	}STUN_DIR;
	
	static const char	stun_dsc[6][25]= {
		"NON-STUN",
		"PORT RESTRICTED CONE", 
		"RESTRICTED CONE", 
		"FULL CONE", 
		"SYMMETRIC NAT", 
		"UNKNOWN NAT TYPE"
	};
#endif //__NF_STUN_COMMON
