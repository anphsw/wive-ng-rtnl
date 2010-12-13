#ifndef __LINUX_NETFILTER_H
#define __LINUX_NETFILTER_H

#include <linux/types.h>

/* Responses from hook functions. */
#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4
#if defined(CONFIG_BCM_NAT) || defined(CONFIG_BCM_NAT_MODULE)
#define NF_FAST_NAT 5
#define NF_STOP 6
#else
#define NF_STOP 5
#endif
#define NF_MAX_VERDICT NF_STOP

/* we overload the higher bits for encoding auxiliary data such as the queue
 * number. Not nice, but better than additional function arguments. */
#define NF_VERDICT_MASK 0x0000ffff
#define NF_VERDICT_BITS 16

#define NF_VERDICT_QMASK 0xffff0000
#define NF_VERDICT_QBITS 16

#define NF_QUEUE_NR(x) (((x << NF_VERDICT_QBITS) & NF_VERDICT_QMASK) | NF_QUEUE)

/* only for userspace compatibility */
/* Generic cache responses from hook functions.
   <= 0x2000 is used for protocol-flags. */
#define NFC_UNKNOWN 0x4000
#define NFC_ALTERED 0x8000

#endif /*__LINUX_NETFILTER_H*/
