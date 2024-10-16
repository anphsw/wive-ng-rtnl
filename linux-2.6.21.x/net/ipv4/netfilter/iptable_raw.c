/*
 * 'raw' table, which is the very first hooked in at PRE_ROUTING and LOCAL_OUT .
 *
 * Copyright (C) 2003 Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 */
#include <linux/module.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#define RAW_VALID_HOOKS ((1 << NF_IP_PRE_ROUTING) | (1 << NF_IP_LOCAL_OUT))

static struct
{
	struct ipt_replace repl;
	struct ipt_standard entries[2];
	struct ipt_error term;
} initial_table __initdata = {
	.repl = {
		.name = "raw",
		.valid_hooks = RAW_VALID_HOOKS,
		.num_entries = 3,
		.size = sizeof(struct ipt_standard) * 2 + sizeof(struct ipt_error),
		.hook_entry = {
			[NF_IP_PRE_ROUTING] = 0,
			[NF_IP_LOCAL_OUT] = sizeof(struct ipt_standard) },
		.underflow = {
			[NF_IP_PRE_ROUTING] = 0,
			[NF_IP_LOCAL_OUT]  = sizeof(struct ipt_standard) },
	},
	.entries = {
	     /* PRE_ROUTING */
	     {
		     .entry = {
			     .target_offset = sizeof(struct ipt_entry),
			     .next_offset = sizeof(struct ipt_standard),
		     },
		     .target = {
			  .target = {
				  .u = {
					  .target_size = IPT_ALIGN(sizeof(struct ipt_standard_target)),
				  },
			  },
			  .verdict = -NF_ACCEPT - 1,
		     },
	     },

	     /* LOCAL_OUT */
	     {
		     .entry = {
			     .target_offset = sizeof(struct ipt_entry),
			     .next_offset = sizeof(struct ipt_standard),
		     },
		     .target = {
			     .target = {
				     .u = {
					     .target_size = IPT_ALIGN(sizeof(struct ipt_standard_target)),
				     },
			     },
			     .verdict = -NF_ACCEPT - 1,
		     },
	     },
	},
	/* ERROR */
	.term = {
		.entry = {
			.target_offset = sizeof(struct ipt_entry),
			.next_offset = sizeof(struct ipt_error),
		},
		.target = {
			.target = {
				.u = {
					.user = {
						.target_size = IPT_ALIGN(sizeof(struct ipt_error_target)),
						.name = IPT_ERROR_TARGET,
					},
				},
			},
			.errorname = "ERROR",
		},
	}
};

static struct xt_table packet_raw = {
	.name = "raw",
	.valid_hooks =  RAW_VALID_HOOKS,
	.me = THIS_MODULE,
	.af = AF_INET,
};

/* The work comes in here from netfilter.c. */
static unsigned int
ipt_hook(unsigned int hook,
	 struct sk_buff **pskb,
	 const struct net_device *in,
	 const struct net_device *out,
	 int (*okfn)(struct sk_buff *))
{
	return ipt_do_table(pskb, hook, in, out, &packet_raw);
}

/* 'raw' is the very first table. */
static struct nf_hook_ops ipt_ops[] __read_mostly = {
	{
		.hook = ipt_hook,
		.pf = PF_INET,
		.hooknum = NF_IP_PRE_ROUTING,
		.priority = NF_IP_PRI_RAW,
		.owner = THIS_MODULE,
	},
	{
		.hook = ipt_hook,
		.pf = PF_INET,
		.hooknum = NF_IP_LOCAL_OUT,
		.priority = NF_IP_PRI_RAW,
		.owner = THIS_MODULE,
	},
};

static int __init iptable_raw_init(void)
{
	int ret;

	/* Register table */
	ret = ipt_register_table(&packet_raw, &initial_table.repl);
	if (ret < 0)
		return ret;

	/* Register hooks */
	ret = nf_register_hooks(ipt_ops, ARRAY_SIZE(ipt_ops));
	if (ret < 0)
		ipt_unregister_table(&packet_raw);

	return ret;
}

static void __exit iptable_raw_fini(void)
{
	nf_unregister_hooks(ipt_ops, ARRAY_SIZE(ipt_ops));
	ipt_unregister_table(&packet_raw);
}

module_init(iptable_raw_init);
module_exit(iptable_raw_fini);
MODULE_LICENSE("GPL");
