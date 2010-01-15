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
#ifndef	CSR_NETLINK_H
#define CSR_NETLINK_H

#define	CSR_NETLINK	30
#define	CSR_READ	0
#define	CSR_WRITE	1
#define	CSR_TEST	2

#define RA2882_CSR_GROUP	2882	

typedef struct rt2880_csr_msg {
  	int	enable;
  	char	reg_name[32];
  	unsigned long address;
  	unsigned long default_value;
  	unsigned long reserved_bits;	/* 1 : not reserved, 0 : reserved */
  	unsigned long write_mask;
  	unsigned long write_value;
  	int	status;
} RT2880_CSR_MSG;

int csr_msg_send(RT2880_CSR_MSG* msg);
int csr_msg_recv();
#endif
