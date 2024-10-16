#ifndef _XT_CONNBYTES_H
#define _XT_CONNBYTES_H

enum xt_connbytes_what {
	XT_CONNBYTES_PKTS,
	XT_CONNBYTES_BYTES,
	XT_CONNBYTES_AVGPKT,
};

enum xt_connbytes_direction {
	XT_CONNBYTES_DIR_ORIGINAL,
	XT_CONNBYTES_DIR_REPLY,
	XT_CONNBYTES_DIR_BOTH,
};

struct xt_connbytes_info {
	struct {
		aligned_u64 from;	/* count to be matched */
		aligned_u64 to;		/* count to be matched */
	} count;
	u_int8_t what;		/* ipt_connbytes_what */
	u_int8_t direction;	/* ipt_connbytes_direction */
};
#endif
