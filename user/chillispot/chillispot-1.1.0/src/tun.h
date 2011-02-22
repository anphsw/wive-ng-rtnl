/* 
 * TUN interface functions.
 *
 * Copyright (c) 2006, Jens Jakobsen 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the names of copyright holders nor the names of its contributors
 *   may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Copyright (C) 2002, 2003, 2004, 2005 Mondru AB.
 * 
 * The contents of this file may be used under the terms of the GNU
 * General Public License Version 2, provided that the above copyright
 * notice and this permission notice is included in all copies or
 * substantial portions of the software.
 * 
 */

#ifndef _TUN_H
#define _TUN_H

#define PACKET_MAX      8196 /* Maximum packet size we receive */
#define TUN_SCRIPTSIZE   256
#define TUN_ADDRSIZE     128
#define TUN_NLBUFSIZE   1024

struct tun_packet_t {
  unsigned int ver:4;
  unsigned int ihl:4;
  unsigned int dscp:6;
  unsigned int ecn:2;
  unsigned int length:16;
  unsigned int id:16;
  unsigned int flags:3;
  unsigned int fragment:13;
  unsigned int ttl:8;
  unsigned int protocol:8;
  unsigned int check:16;
  unsigned int src:32;
  unsigned int dst:32;
};


/* ***********************************************************
 * Information storage for each tun instance
 *************************************************************/

struct tun_t {
  int fd;                /* File descriptor to tun interface */
  struct in_addr addr;
  struct in_addr dstaddr;
  struct in_addr netmask;
  int addrs;             /* Number of allocated IP addresses */
  int routes;            /* One if we allocated an automatic route */
  char devname[IFNAMSIZ];/* Name of the tun device */
  int (*cb_ind) (struct tun_t *tun, void *pack, unsigned len);
};


extern int tun_new(struct tun_t **tun);
extern int tun_free(struct tun_t *tun);
extern int tun_decaps(struct tun_t *this);
extern int tun_encaps(struct tun_t *tun, void *pack, unsigned len);

extern int tun_addaddr(struct tun_t *this, struct in_addr *addr,
		       struct in_addr *dstaddr, struct in_addr *netmask);


extern int tun_setaddr(struct tun_t *this, struct in_addr *our_adr, 
		       struct in_addr *his_adr, struct in_addr *net_mask);

int tun_addroute(struct tun_t *this, struct in_addr *dst, 
		 struct in_addr *gateway, struct in_addr *mask);

extern int tun_set_cb_ind(struct tun_t *this, 
     int (*cb_ind) (struct tun_t *tun, void *pack, unsigned len));


extern int tun_runscript(struct tun_t *tun, char* script);

#endif	/* !_TUN_H */
