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
/* Shared library add-on to iptables for TARPIT support */
#include <stdio.h>
#include <getopt.h>
#include <iptables.h>

static void
help(void)
{
	fputs(
"TARPIT takes no options\n"
"\n", stdout);
}

static struct option opts[] = {
	{ 0 }
};

static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const struct ipt_entry *entry,
      struct ipt_entry_target **target)
{
	return 0;
}

static void final_check(unsigned int flags)
{
}

static void
print(const struct ipt_ip *ip,
      const struct ipt_entry_target *target,
      int numeric)
{
}

static void save(const struct ipt_ip *ip, const struct ipt_entry_target *target)
{
}

static struct iptables_target tarpit = {
	.next		= NULL,
	.name		= "TARPIT",
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
	register_target(&tarpit);
}
