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
/*
 * decodenetnum - return a net number (this is crude, but careful)
 */
#include <sys/types.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ntp_stdlib.h"

int
decodenetnum(
	const char *num,
	struct sockaddr_storage *netnum
	)
{
	struct addrinfo hints, *ai = NULL;
	register int err, i;
	register const char *cp;
	char name[80];

	cp = num;

	if (*cp == '[') {
		cp++;
		for (i = 0; *cp != ']'; cp++, i++)
			name[i] = *cp;
	name[i] = '\0';
	num = name; 
	}
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_NUMERICHOST;
	err = getaddrinfo(num, NULL, &hints, &ai);
	if (err != 0)
		return 0;
	memcpy(netnum, (struct sockaddr_storage *)ai->ai_addr, ai->ai_addrlen); 
	freeaddrinfo(ai);
	return 1;
}
