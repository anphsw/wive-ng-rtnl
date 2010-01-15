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
/*
 * static_leases.c -- Couple of functions to assist with storing and
 * retrieving data for static leases
 *
 * Wade Berrier <wberrier@myrealbox.com> September 2004
 *
 */

#include "common.h"
#include "dhcpd.h"


/* Takes the address of the pointer to the static_leases linked list,
 *   Address to a 6 byte mac address
 *   Address to a 4 byte ip address */
int addStaticLease(struct static_lease **lease_struct, uint8_t *mac, uint32_t *ip)
{
	struct static_lease *cur;
	struct static_lease *new_static_lease;

	/* Build new node */
	new_static_lease = xmalloc(sizeof(struct static_lease));
	new_static_lease->mac = mac;
	new_static_lease->ip = ip;
	new_static_lease->next = NULL;

	/* If it's the first node to be added... */
	if (*lease_struct == NULL) {
		*lease_struct = new_static_lease;
	} else {
		cur = *lease_struct;
		while (cur->next) {
			cur = cur->next;
		}

		cur->next = new_static_lease;
	}

	return 1;
}

/* Check to see if a mac has an associated static lease */
uint32_t getIpByMac(struct static_lease *lease_struct, void *arg)
{
	uint32_t return_ip;
	struct static_lease *cur = lease_struct;
	uint8_t *mac = arg;

	return_ip = 0;

	while (cur) {
		/* If the client has the correct mac  */
		if (memcmp(cur->mac, mac, 6) == 0) {
			return_ip = *(cur->ip);
		}

		cur = cur->next;
	}

	return return_ip;
}

/* Check to see if an ip is reserved as a static ip */
uint32_t reservedIp(struct static_lease *lease_struct, uint32_t ip)
{
	struct static_lease *cur = lease_struct;

	uint32_t return_val = 0;

	while (cur) {
		/* If the client has the correct ip  */
		if (*cur->ip == ip)
			return_val = 1;

		cur = cur->next;
	}

	return return_val;
}

#if ENABLE_FEATURE_UDHCP_DEBUG
/* Print out static leases just to check what's going on */
/* Takes the address of the pointer to the static_leases linked list */
void printStaticLeases(struct static_lease **arg)
{
	/* Get a pointer to the linked list */
	struct static_lease *cur = *arg;

	while (cur) {
		/* printf("PrintStaticLeases: Lease mac Address: %x\n", cur->mac); */
		printf("PrintStaticLeases: Lease mac Value: %x\n", *(cur->mac));
		/* printf("PrintStaticLeases: Lease ip Address: %x\n", cur->ip); */
		printf("PrintStaticLeases: Lease ip Value: %x\n", *(cur->ip));

		cur = cur->next;
	}
}
#endif
