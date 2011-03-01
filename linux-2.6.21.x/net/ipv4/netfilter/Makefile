#
# Makefile for the netfilter modules on top of IPv4.
#

# objects for the standalone - connection tracking / NAT
ip_conntrack-objs	:= ip_conntrack_standalone.o ip_conntrack_core.o ip_conntrack_proto_generic.o ip_conntrack_proto_tcp.o ip_conntrack_proto_udp.o ip_conntrack_proto_icmp.o
# objects for l3 independent conntrack
nf_conntrack_ipv4-objs  :=  nf_conntrack_l3proto_ipv4.o nf_conntrack_proto_icmp.o
ifeq ($(CONFIG_NF_CONNTRACK_PROC_COMPAT),y)
ifeq ($(CONFIG_PROC_FS),y)
nf_conntrack_ipv4-objs	+= nf_conntrack_l3proto_ipv4_compat.o
endif
endif

ip_nat-objs	:= ip_nat_core.o ip_nat_helper.o ip_nat_proto_unknown.o ip_nat_proto_tcp.o ip_nat_proto_udp.o ip_nat_proto_icmp.o
nf_nat-objs	:= nf_nat_core.o nf_nat_helper.o nf_nat_proto_unknown.o nf_nat_proto_tcp.o nf_nat_proto_udp.o nf_nat_proto_icmp.o

ifneq ($(CONFIG_NF_NAT),)
iptable_nat-objs	:= nf_nat_rule.o nf_nat_standalone.o
else
iptable_nat-objs	:= ip_nat_rule.o ip_nat_standalone.o
endif

ifeq ($(CONFIG_NF_NAT),)
ip_conntrack_pptp-objs	:= ip_conntrack_helper_pptp.o ip_conntrack_proto_gre.o
ip_nat_pptp-objs	:= ip_nat_helper_pptp.o ip_nat_proto_gre.o
else
obj-$(CONFIG_NF_NAT_PROTO_GRE) += nf_nat_proto_gre.o
endif

ip_conntrack_h323-objs := ip_conntrack_helper_h323.o ../../netfilter/nf_conntrack_h323_asn1.o
ip_nat_h323-objs := ip_nat_helper_h323.o

# connection tracking
obj-$(CONFIG_IP_NF_CONNTRACK) += ip_conntrack.o
obj-$(CONFIG_NF_CONNTRACK_IPV4) += nf_conntrack_ipv4.o

obj-$(CONFIG_IP_NF_NAT) += ip_nat.o
obj-$(CONFIG_NF_NAT) += nf_nat.o

# conntrack netlink interface
obj-$(CONFIG_IP_NF_CONNTRACK_NETLINK) += ip_conntrack_netlink.o


# SCTP protocol connection tracking
obj-$(CONFIG_IP_NF_CT_PROTO_SCTP) += ip_conntrack_proto_sctp.o

# connection tracking helpers
obj-$(CONFIG_IP_NF_H323) += ip_conntrack_h323.o
obj-$(CONFIG_IP_NF_AMANDA) += ip_conntrack_amanda.o
obj-$(CONFIG_IP_NF_TFTP) += ip_conntrack_tftp.o
obj-$(CONFIG_IP_NF_FTP) += ip_conntrack_ftp.o
obj-$(CONFIG_IP_NF_EGG) += ip_conntrack_egg.o
obj-$(CONFIG_IP_NF_IRC) += ip_conntrack_irc.o
obj-$(CONFIG_IP_NF_SIP) += ip_conntrack_sip.o
obj-$(CONFIG_IP_NF_NETBIOS_NS) += ip_conntrack_netbios_ns.o
obj-$(CONFIG_IP_NF_MMS) += ip_conntrack_mms.o
obj-$(CONFIG_IP_NF_TALK) += ip_conntrack_talk.o
obj-$(CONFIG_IP_NF_QUAKE3) += ip_conntrack_quake3.o

# rtsp protocol support
obj-$(CONFIG_IP_NF_RTSP)  += ip_conntrack_rtsp.o
obj-$(CONFIG_IP_NF_NAT_RTSP)  += ip_nat_rtsp.o

# l2tp protocol support
obj-$(CONFIG_IP_NF_L2TP)  += ip_conntrack_l2tp.o

# NAT helpers (ip_conntrack)
obj-$(CONFIG_IP_NF_NAT_H323) += ip_nat_h323.o
obj-$(CONFIG_IP_NF_NAT_PPTP) += ip_nat_pptp.o
obj-$(CONFIG_IP_NF_NAT_AMANDA) += ip_nat_amanda.o
obj-$(CONFIG_IP_NF_NAT_TFTP) += ip_nat_tftp.o
obj-$(CONFIG_IP_NF_NAT_FTP) += ip_nat_ftp.o
obj-$(CONFIG_IP_NF_NAT_IRC) += ip_nat_irc.o
obj-$(CONFIG_IP_NF_NAT_QUAKE3) += ip_nat_quake3.o
obj-$(CONFIG_IP_NF_NAT_SIP) += ip_nat_sip.o
obj-$(CONFIG_IP_NF_NAT_MMS) += ip_nat_mms.o
obj-$(CONFIG_IP_NF_NAT_TALK) += ip_nat_talk.o

# NAT helpers (nf_conntrack)
obj-$(CONFIG_NF_NAT_AMANDA) += nf_nat_amanda.o
obj-$(CONFIG_NF_NAT_FTP) += nf_nat_ftp.o
obj-$(CONFIG_NF_NAT_H323) += nf_nat_h323.o
obj-$(CONFIG_NF_NAT_IRC) += nf_nat_irc.o
obj-$(CONFIG_NF_NAT_RTSP) += nf_nat_rtsp.o
obj-$(CONFIG_NF_NAT_SIP) += nf_nat_sip.o
obj-$(CONFIG_NF_NAT_SNMP_BASIC) += nf_nat_snmp_basic.o
obj-$(CONFIG_NF_NAT_TFTP) += nf_nat_tftp.o

# NAT protocols (nf_nat)
ifeq ($(CONFIG_NF_NAT),)
obj-$(CONFIG_IP_NF_PPTP) += ip_conntrack_pptp.o
else
obj-$(CONFIG_NF_NAT_PPTP) += nf_nat_pptp.o
endif

# generic IP tables 
obj-$(CONFIG_IP_NF_IPTABLES) += ip_tables.o

# the three instances of ip_tables
obj-$(CONFIG_IP_NF_FILTER) += iptable_filter.o
obj-$(CONFIG_IP_NF_MANGLE) += iptable_mangle.o
obj-$(CONFIG_IP_NF_NAT) += iptable_nat.o
obj-$(CONFIG_NF_NAT) += iptable_nat.o
obj-$(CONFIG_IP_NF_RAW) += iptable_raw.o

# matches
obj-$(CONFIG_IP_NF_MATCH_IPRANGE) += ipt_iprange.o
obj-$(CONFIG_IP_NF_MATCH_OWNER) += ipt_owner.o
obj-$(CONFIG_IP_NF_MATCH_TOS) += ipt_tos.o
obj-$(CONFIG_IP_NF_MATCH_CONNLIMIT) += ipt_connlimit.o

obj-$(CONFIG_IP_NF_MATCH_IPV4OPTIONS) += ipt_ipv4options.o

obj-$(CONFIG_IP_NF_MATCH_TIME) += ipt_time.o
obj-$(CONFIG_IP_NF_MATCH_RECENT) += ipt_recent.o
obj-$(CONFIG_IP_NF_MATCH_IPP2P) += ipt_ipp2p.o
obj-$(CONFIG_IP_NF_MATCH_U32) += ipt_u32.o
obj-$(CONFIG_IP_NF_MATCH_ECN) += ipt_ecn.o
obj-$(CONFIG_IP_NF_MATCH_AH) += ipt_ah.o
obj-$(CONFIG_IP_NF_MATCH_TTL) += ipt_ttl.o
obj-$(CONFIG_IP_NF_MATCH_SET) += ipt_set.o
obj-$(CONFIG_IP_NF_MATCH_ADDRTYPE) += ipt_addrtype.o

obj-$(CONFIG_IP_NF_MATCH_ACCOUNT) += ipt_account.o

# targets
obj-$(CONFIG_IP_NF_TARGET_REJECT) += ipt_REJECT.o
obj-$(CONFIG_IP_NF_TARGET_TARPIT) += ipt_TARPIT.o
obj-$(CONFIG_IP_NF_TARGET_TOS) += ipt_TOS.o
obj-$(CONFIG_IP_NF_TARGET_ECN) += ipt_ECN.o
obj-$(CONFIG_IP_NF_TARGET_IMQ) += ipt_IMQ.o
obj-$(CONFIG_IP_NF_TARGET_MASQUERADE) += ipt_MASQUERADE.o
obj-$(CONFIG_IP_NF_TARGET_REDIRECT) += ipt_REDIRECT.o
obj-$(CONFIG_IP_NF_TARGET_NETMAP) += ipt_NETMAP.o
obj-$(CONFIG_IP_NF_TARGET_SAME) += ipt_SAME.o
obj-$(CONFIG_IP_NF_NAT_SNMP_BASIC) += ip_nat_snmp_basic.o
obj-$(CONFIG_IP_NF_TARGET_LOG) += ipt_LOG.o
obj-$(CONFIG_IP_NF_TARGET_IPV4OPTSSTRIP) += ipt_IPV4OPTSSTRIP.o
obj-$(CONFIG_IP_NF_TARGET_ULOG) += ipt_ULOG.o
obj-$(CONFIG_IP_NF_TARGET_CLUSTERIP) += ipt_CLUSTERIP.o
obj-$(CONFIG_IP_NF_TARGET_TTL) += ipt_TTL.o
obj-$(CONFIG_IP_NF_TARGET_SET) += ipt_SET.o

# sets
obj-$(CONFIG_IP_NF_SET) += ip_set.o
obj-$(CONFIG_IP_NF_SET_IPMAP) += ip_set_ipmap.o
obj-$(CONFIG_IP_NF_SET_PORTMAP) += ip_set_portmap.o
obj-$(CONFIG_IP_NF_SET_MACIPMAP) += ip_set_macipmap.o
obj-$(CONFIG_IP_NF_SET_IPHASH) += ip_set_iphash.o
obj-$(CONFIG_IP_NF_SET_NETHASH) += ip_set_nethash.o
obj-$(CONFIG_IP_NF_SET_IPPORTHASH) += ip_set_ipporthash.o
obj-$(CONFIG_IP_NF_SET_IPTREE) += ip_set_iptree.o
obj-$(CONFIG_IP_NF_SET_IPTREEMAP) += ip_set_iptreemap.o

# generic ARP tables
obj-$(CONFIG_IP_NF_ARPTABLES) += arp_tables.o
obj-$(CONFIG_IP_NF_ARP_MANGLE) += arpt_mangle.o

# just filtering instance of ARP tables for now
obj-$(CONFIG_IP_NF_ARPFILTER) += arptable_filter.o

obj-$(CONFIG_IP_NF_QUEUE) += ip_queue.o

# Broadcom NAT
obj-$(CONFIG_BCM_NAT) += bcm_nat.o
