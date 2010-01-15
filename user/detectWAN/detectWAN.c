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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <nvram/bcmnvram.h>     //Alicia_Wang

#include "detectWAN.h"

#define csprintf(fmt, args...) do{\
	FILE *cp = fopen("/dev/console", "w");\
	if(cp) {\
		fprintf(cp, fmt, ## args);\
		fclose(cp);\
	}\
}while(0)

static int create_icmp_socket(void)
{
	struct protoent *proto;
	int sock;
	
	proto = getprotobyname("icmp");
	if((sock = socket(AF_INET, SOCK_RAW, (proto?proto->p_proto:1))) < 0){
		if(errno == EPERM)
			csprintf("Permission denied.");
		else
			csprintf("Can not create raw socket!\n");
		
		return -1;
	}
	
	/* drop root privs if running setuid */
	setuid(getuid());
	
	return sock;
}

static int send_request(int seq, int ttl)
{
	struct opacket *op = outpacket;
	struct ip *ip = &op->ip;
	struct udphdr *udp = &op->udp;
	int i;
	struct timezone tz;
	
	ident = (getpid()&0xffff)|0x8000;
	
	// set the output packet
	ip->ip_v = IPVERSION;
	ip->ip_hl = sizeof(*ip)>>2;
	ip->ip_tos = 0;
	ip->ip_len = datalen;
	ip->ip_id = htons(ident+seq);
	ip->ip_off = 0;
	ip->ip_ttl = ttl;
	ip->ip_p = IPPROTO_UDP;
	ip->ip_dst = ((struct sockaddr_in *)&whereto)->sin_addr;
	
	udp->source = htons(ident);
	udp->dest = htons(port+seq);
	udp->len = htons((u_short)(datalen-sizeof(struct ip)));
	udp->check = 0;
	
	op->seq = seq;
	op->ttl = ttl;
	gettimeofday(&op->tv, &tz);
	
	i = sendto(sendsock, (char *)outpacket, datalen, 0, &whereto, sizeof(struct sockaddr));
	if(i < 0 || i != datalen){
		if(i < 0)
			perror("sendto");
		csprintf("traceroute: wrote %s %d chars, ret=%d\n", hostname, datalen, i);
		fflush(stdout);
		
		return -1;
	}
	
	return 0;
}

static int wait_response(int sock, struct sockaddr_in *from, int reset_timer)
{
	fd_set fds;
	int recv_len = 0;
	int fromlen = sizeof(*from);
	
	FD_ZERO(&fds);
	FD_SET(sock, &fds);
	
	if(reset_timer){
		wait_timeval.tv_sec = waittime;
		wait_timeval.tv_usec = 0;
	}
	
	if(select(sock+1, &fds, (fd_set *)0, (fd_set *)0, &wait_timeval) > 0)
		recv_len = recvfrom(sock, (char *)inputpacket, sizeof(inputpacket), 0, (struct sockaddr *)from, &fromlen);

	return recv_len;
}

static int check_response(u_char *buf, int recv_len, int seq)
{
	struct ip *ip, *hip;
	struct icmp *icp;
	struct udphdr *up;
	u_char type, code;
	int hlen;
	
	ip = (struct ip *)buf;
	hlen = ip->ip_hl<<2;
	if(recv_len < hlen+ICMP_MINLEN)
		return 0;
	
	recv_len -= hlen;
	icp = (struct icmp *)(buf+hlen);
	type = icp->icmp_type;
	code = icp->icmp_code;
	if((type == ICMP_TIMXCEED && code == ICMP_TIMXCEED_INTRANS)
			|| type == ICMP_UNREACH){
		
		hip = &icp->icmp_ip;
		hlen = hip->ip_hl<<2;
		up = (struct udphdr *)((u_char *)hip+hlen);
		if(hlen+12 <= recv_len
				&& hip->ip_p == IPPROTO_UDP
				&& up->source == htons(ident)
				&& up->dest == htons(port+seq)
				)
			return (type == ICMP_TIMXCEED?-1:code);
	}
	
	return 0;
}

static int check_out_of_lan(const char *target_ip)
{
	unsigned long ip;
	
	ip = inet_network(target_ip);
	
	if(ip < CLASS_B_HEAD){
		if(ip >= CLASS_A_PRIVATE_HEAD && ip <= CLASS_A_PRIVATE_TAIL)
			return 0;
	}
	else if(ip < CLASS_C_HEAD){
		if(ip >= CLASS_B_PRIVATE_HEAD && ip <= CLASS_B_PRIVATE_TAIL)
			return 0;
	}
	else if(ip < CLASS_D_HEAD){
		if(ip >= CLASS_C_PRIVATE_HEAD && ip <= CLASS_C_PRIVATE_TAIL)
			return 0;
	}
	
	return 1;
}

static void close_socket()
{
	if(sendsock != -1){
		close(sendsock);
		sendsock = -1;
	}
	
	if(recvsock != -1){
		close(recvsock);
		recvsock = -1;
	}
	
	if(outpacket != NULL)
		free(outpacket);
}

extern int detectWAN()
{
	struct hostent *hp;
	struct sockaddr_in from, *to, lastaddr;
	struct ip *ip;
	int ttl, seq = 0;
	int recv_len, reset_timer;
	int ret;
	int got_there = 0, out_of_lan = 0, got_no_host = 0;
	int unreachable = -1;
	
	// set destination socket's addr
	memset(&whereto, 0, sizeof(struct sockaddr));
	to = (struct sockaddr_in *)&whereto;
	hp = gethostbyname(remote_addr);
	hostname = (char *)hp->h_name;
	to->sin_family = hp->h_addrtype;
	memcpy(&to->sin_addr, hp->h_addr, hp->h_length);
	
	// initialize the output packet
	datalen = sizeof(struct opacket);
	outpacket = (struct opacket *)malloc(datalen);
	if(outpacket == NULL){
		csprintf("Can't malloc outpacket!\n");
		return -1;
	}
	memset(outpacket, 0, datalen);
	
	// build sockets.
	if((sendsock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0){
		csprintf("Can not create raw socket!\n");
		free(outpacket);
		return -1;
	}
	recvsock = create_icmp_socket();
	if(recvsock == -1){
		close_socket();
		return -1;
	}
	
	for(ttl = 1; ttl <= max_ttl; ++ttl){
		ret = send_request(++seq, ttl);
		if(ret < 0){
			close_socket();
			return -1;
		}
		
		reset_timer = 1;
		memset(&from, 0, sizeof(struct sockaddr_in));
		memset(&lastaddr, 0, sizeof(struct sockaddr_in));
		while((recv_len = wait_response(recvsock, &from, reset_timer)) != 0){
			if((ret = check_response(inputpacket, recv_len, seq))){
				reset_timer = 1;
				if(strcmp(inet_ntoa(from.sin_addr), inet_ntoa(lastaddr.sin_addr))){
					csprintf("%2d  %s != %s", ttl, inet_ntoa(from.sin_addr), inet_ntoa(lastaddr.sin_addr));
					if(check_out_of_lan(inet_ntoa(from.sin_addr))){
						++out_of_lan;
						break;
					}
					memcpy(&lastaddr, &from, sizeof(struct sockaddr_in));
				}
				
				switch(ret){
					case ICMP_UNREACH_NET:
						++unreachable;
						csprintf(" !N");
						break;
					case ICMP_UNREACH_HOST:
						++unreachable;
						csprintf(" !H");
						break;
					case ICMP_UNREACH_PROTOCOL:
						++got_there;
						csprintf(" !P");
						break;
					case ICMP_UNREACH_PORT:
						ip = (struct ip *)inputpacket;
						++got_there;
						if(ip->ip_ttl <= 1)
							csprintf(" !");
						break;
					case ICMP_UNREACH_NEEDFRAG:
						++unreachable;
						csprintf(" !F");
						break;
					case ICMP_UNREACH_SRCFAIL:
						++unreachable;
						csprintf(" !S");
						break;
				}
				break;
			}
			else
				reset_timer = 0;
		}
		if(recv_len == 0){
			++got_no_host;
			csprintf(" *");
		}
		fflush(stdout);
		csprintf("\n");
		
		if(got_there || out_of_lan){
			close_socket();
			return 0;
		}
		else if(unreachable >= 0){
			close_socket();
			return -1;
		}
	}
	
	close_socket();
	csprintf("got_no_host=%d.\n", got_no_host);
	if(got_no_host != max_ttl)
		return 0;
	else
		return -1;
}

/*int main(int argc, char *argv[])
{
	int ret;
	
	ret = detectWAN();
	
	if(ret < 0)
		csprintf("Failure!\n");
	else
		csprintf("Success!\n");
	
	return 0;
}*/
