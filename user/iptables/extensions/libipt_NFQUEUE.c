/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/* Shared library add-on to iptables for NFQ
 *
 * (C) 2005 by Harald Welte <laforge@netfilter.org>
 *
 * This program is distributed under the terms of GNU GPL v2, 1991
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <iptables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_NFQUEUE.h>

static void init(struct ipt_entry_target *t, unsigned int *nfcache) 
{
}

static void help(void) 
{
	printf(
"NFQUEUE target options\n"
"  --queue-num value		Send packet to QUEUE number <value>.\n"
"  		                Valid queue numbers are 0-65535\n"
);
}

static struct option opts[] = {
	{ "queue-num", 1, 0, 'F' },
	{ 0 }
};

static void
parse_num(const char *s, struct ipt_NFQ_info *tinfo)
{
	unsigned int num;
       
	if (string_to_number(s, 0, 65535, &num) == -1)
		exit_error(PARAMETER_PROBLEM,
			   "Invalid queue number `%s'\n", s);

    	tinfo->queuenum = num & 0xffff;
    	return;
}

static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      struct ipt_entry_target **target)
{
	struct ipt_NFQ_info *tinfo
		= (struct ipt_NFQ_info *)(*target)->data;

	switch (c) {
	case 'F':
		if (*flags)
			exit_error(PARAMETER_PROBLEM, "NFQUEUE target: "
				   "Only use --queue-num ONCE!");
		parse_num(optarg, tinfo);
		break;
	default:
		return 0;
	}

	return 1;
}

static void
final_check(unsigned int flags)
{
}

/* Prints out the targinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_target *target,
      int numeric)
{
	const struct ipt_NFQ_info *tinfo =
		(const struct ipt_NFQ_info *)target->data;
	printf("NFQUEUE num %u", tinfo->queuenum);
}

/* Saves the union ipt_targinfo in parsable form to stdout. */
static void
save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
	const struct ipt_NFQ_info *tinfo =
		(const struct ipt_NFQ_info *)target->data;

	printf("--queue-num %u ", tinfo->queuenum);
}

static struct iptables_target nfqueue = { 
	.next		= NULL,
	.name		= "NFQUEUE",
	.version	= IPTABLES_VERSION,
	.size		= IPT_ALIGN(sizeof(struct ipt_NFQ_info)),
	.userspacesize	= IPT_ALIGN(sizeof(struct ipt_NFQ_info)),
	.help		= &help,
	.init		= &init,
	.parse		= &parse,
	.final_check	= &final_check,
	.print		= &print,
	.save		= &save,
	.extra_opts	= opts
};

void _init(void)
{
	register_target(&nfqueue);
}
