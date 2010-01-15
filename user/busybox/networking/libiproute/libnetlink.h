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
#ifndef __LIBNETLINK_H__
#define __LIBNETLINK_H__ 1

#include <linux/types.h>
/* We need linux/types.h because older kernels use __u32 etc
 * in linux/[rt]netlink.h. 2.6.19 seems to be ok, though */
#include <linux/netlink.h>
#include <linux/rtnetlink.h>


#if __GNUC_PREREQ(4,1)
# pragma GCC visibility push(hidden)
#endif

struct rtnl_handle
{
	int			fd;
	struct sockaddr_nl	local;
	struct sockaddr_nl	peer;
	uint32_t		seq;
	uint32_t		dump;
};

extern int xrtnl_open(struct rtnl_handle *rth) FAST_FUNC;
extern void rtnl_close(struct rtnl_handle *rth) FAST_FUNC;
extern int xrtnl_wilddump_request(struct rtnl_handle *rth, int fam, int type) FAST_FUNC;
extern int rtnl_dump_request(struct rtnl_handle *rth, int type, void *req, int len) FAST_FUNC;
extern int xrtnl_dump_filter(struct rtnl_handle *rth,
			int (*filter)(const struct sockaddr_nl*, struct nlmsghdr *n, void*),
			void *arg1) FAST_FUNC;

/* bbox doesn't use parameters no. 3, 4, 6, 7, stub them out */
#define rtnl_talk(rtnl, n, peer, groups, answer, junk, jarg) \
	rtnl_talk(rtnl, n, answer)
extern int rtnl_talk(struct rtnl_handle *rtnl, struct nlmsghdr *n, pid_t peer,
			unsigned groups, struct nlmsghdr *answer,
			int (*junk)(struct sockaddr_nl *,struct nlmsghdr *n, void *),
			void *jarg) FAST_FUNC;

extern int rtnl_send(struct rtnl_handle *rth, char *buf, int) FAST_FUNC;


extern int addattr32(struct nlmsghdr *n, int maxlen, int type, uint32_t data) FAST_FUNC;
extern int addattr_l(struct nlmsghdr *n, int maxlen, int type, void *data, int alen) FAST_FUNC;
extern int rta_addattr32(struct rtattr *rta, int maxlen, int type, uint32_t data) FAST_FUNC;
extern int rta_addattr_l(struct rtattr *rta, int maxlen, int type, void *data, int alen) FAST_FUNC;

extern int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len) FAST_FUNC;

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility pop
#endif

#endif /* __LIBNETLINK_H__ */
