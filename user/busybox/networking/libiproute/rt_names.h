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
#ifndef RT_NAMES_H_
#define RT_NAMES_H_ 1

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility push(hidden)
#endif

extern const char* rtnl_rtprot_n2a(int id, char *buf, int len);
extern const char* rtnl_rtscope_n2a(int id, char *buf, int len);
extern const char* rtnl_rtrealm_n2a(int id, char *buf, int len);
extern const char* rtnl_dsfield_n2a(int id, char *buf, int len);
extern const char* rtnl_rttable_n2a(int id, char *buf, int len);
extern int rtnl_rtprot_a2n(uint32_t *id, char *arg);
extern int rtnl_rtscope_a2n(uint32_t *id, char *arg);
extern int rtnl_rtrealm_a2n(uint32_t *id, char *arg);
extern int rtnl_dsfield_a2n(uint32_t *id, char *arg);
extern int rtnl_rttable_a2n(uint32_t *id, char *arg);


extern const char* ll_type_n2a(int type, char *buf, int len);

extern const char* ll_addr_n2a(unsigned char *addr, int alen, int type,
				char *buf, int blen);
extern int ll_addr_a2n(unsigned char *lladdr, int len, char *arg);

#ifdef UNUSED
extern const char* ll_proto_n2a(unsigned short id, char *buf, int len);
extern int ll_proto_a2n(unsigned short *id, char *buf);
#endif

#if __GNUC_PREREQ(4,1)
# pragma GCC visibility pop
#endif

#endif
