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
/* vi: set sw=4 ts=4: */
#ifndef __LL_MAP_H__
#define __LL_MAP_H__ 1

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility push(hidden)
#endif

int ll_remember_index(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg);
int ll_init_map(struct rtnl_handle *rth);
int xll_name_to_index(const char *const name);
const char *ll_index_to_name(int idx);
const char *ll_idx_n2a(int idx, char *buf);
/* int ll_index_to_type(int idx); */
unsigned ll_index_to_flags(int idx);

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility pop
#endif

#endif /* __LL_MAP_H__ */
