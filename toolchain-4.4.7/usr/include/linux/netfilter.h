#ifndef __LINUX_NETFILTER_H
#define __LINUX_NETFILTER_H

#include <linux/types.h>

/* Nat mode type */
#ifdef CONFIG_NAT_CONE
#define NAT_MODE_LINUX		0
#define NAT_MODE_FCONE		1
#define NAT_MODE_RCONE		2
#endif

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
 * number or errno values. Not nice, but better than additional function
 * arguments. */
#define NF_VERDICT_MASK 0x000000ff

/* extra verdict flags have mask 0x0000ff00 */

/* queue number (NF_QUEUE) or errno (NF_DROP) */

#define NF_VERDICT_QMASK 0xffff0000
#define NF_VERDICT_QBITS 16

#define NF_QUEUE_NR(x) ((((x) << 16) & NF_VERDICT_QMASK) | NF_QUEUE)

#define NF_DROP_ERR(x) (((-x) << 16) | NF_DROP)

/* only for userspace compatibility */
/* Generic cache responses from hook functions.
   <= 0x2000 is used for protocol-flags. */
#define NFC_UNKNOWN 0x4000
#define NFC_ALTERED 0x8000

/* NF_VERDICT_BITS should be 8 now, but userspace might break if this changes */
#define NF_VERDICT_BITS 16

#endif /*__LINUX_NETFILTER_H*/
