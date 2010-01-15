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
/* Shared library add-on to iptables for rpc match */
#include <stdio.h>
#include <getopt.h>
#include <iptables.h>

/* Function which prints out usage message. */
static void
help(void)
{
	printf(
"record_rpc v%s takes no options\n"
"\n", IPTABLES_VERSION);
}

static struct option opts[] = {
	{0}
};

/* Function which parses command options; returns true if it
   ate an option */
static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      unsigned int *nfcache,
      struct ipt_entry_match **match)
{
	return 0;
}

/* Final check; must have specified --mac. */
static void final_check(unsigned int flags)
{
}

/* Prints out the union ipt_matchinfo. */
static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_match *match,
      int numeric)
{
}

static void save(const struct ipt_ip *ip, const struct ipt_entry_match *match)
{
}

static
struct iptables_match record_rpc = { 
	.next		= NULL,
	.name 		= "record_rpc",
	.version	= IPTABLES_VERSION,
	.size		= IPT_ALIGN(0),
	.userspacesize	= IPT_ALIGN(0),
	.help		= &help,
	.parse		= &parse,
	.final_check	= &final_check,
	.print		= &print,
	.save		= &save,
	.extra_opts	= opts
};

void _init(void)
{
	register_match(&record_rpc);
}
