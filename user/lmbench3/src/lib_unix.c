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
 * unix_lib.c - routines for managing UNIX connections.
 *
 * Positive port/program numbers are RPC ports, negative ones are UNIX ports.
 *
 * Copyright (c) 1994-1996 Larry McVoy.
 */
#define		_LIB /* bench.h needs this */
#include	"bench.h"

/*
 * Get a UNIX socket, bind it.
 */
int
unix_server(char *path)
{
	int	sock;
	struct	sockaddr_un s;

#ifdef	LIBUNIX_VERBOSE
	fprintf(stderr, "unix_server(%s, %u)\n", prog, rdwr);
#endif
	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	bzero((void*)&s, sizeof(s));
	s.sun_family = AF_UNIX;
	strcpy(s.sun_path, path);
	if (bind(sock, (struct sockaddr*)&s, sizeof(s)) < 0) {
		perror("bind");
		exit(2);
	}
	if (listen(sock, 100) < 0) {
		perror("listen");
		exit(4);
	}
	return (sock);
}

/*
 * Unadvertise the socket
 */
int
unix_done(int sock, char *path)
{
	close(sock);
	unlink(path);
	return (0);
}

/*
 * Accept a connection and return it
 */
int
unix_accept(int sock)
{
	struct	sockaddr_un s;
	int	newsock, namelen;

	namelen = sizeof(s);
	bzero((void*)&s, namelen);

retry:
	if ((newsock = accept(sock, (struct sockaddr*)&s, &namelen)) < 0) {
		if (errno == EINTR)
			goto retry;
		perror("accept");
		exit(6);
	}
	return (newsock);
}

/*
 * Connect to the UNIX socket advertised as "path" and
 * return the connected socket.
 */
int
unix_connect(char *path)
{
	struct	sockaddr_un s;
	int	sock;

	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	bzero((void*)&s, sizeof(s));
	s.sun_family = AF_UNIX;
	strcpy(s.sun_path, path);
	if (connect(sock, (struct sockaddr*)&s, sizeof(s)) < 0) {
		perror("connect");
		exit(4);
	}
	return (sock);
}

