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
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

static void catch_sigchild(int sig)
{
	pid_t status;

	printf("MY CHILDREN!\n");

	wait(&status);

	printf("I WILL FOLLOW U\n");
	exit(0);
}

int
main(int argc, char *argv[])
{
	int pid;

	pid = fork();

	if(pid == 0)	// children
	{
		sleep(1);
		printf("children: i am going to die\n");
		exit(0);
	}

	signal(SIGCHLD, catch_sigchild);
	
	for(;;)
	{
		
	}
	
	return 0;
}
