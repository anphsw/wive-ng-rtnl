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
 * Layer Two Tunnelling Protocol Daemon
 * Copyright (C) 1998 Adtran, Inc.
 * Copyright (C) 2002 Jeff McAdams
 *
 * Mark Spencer
 *
 * This software is distributed under the terms
 * of the GPL, which you should have received
 * along with this source.
 *
 * Authorization, Accounting, and Access control
 *
 */

#ifndef _AAA_H
#define _AAA_H
#include "md5.h"

#define ADDR_HASH_SIZE 256
#define MD_SIG_SIZE 16
#define MAX_VECTOR_SIZE 1024
#define VECTOR_SIZE 16

#define STATE_NONE 		 0
#define STATE_CHALLENGED 1
#define STATE_COMPLETE	 2

struct addr_ent
{
    unsigned int addr;
    struct addr_ent *next;
};

struct challenge
{
    struct MD5Context md5;
    unsigned char ss;           /* State we're sending in */
    unsigned char secret[MAXSTRLEN];    /* The shared secret */
    unsigned char *challenge;       /* The original challenge */
    unsigned int chal_len;       /* The length of the original challenge */
    unsigned char response[MD_SIG_SIZE];        /* What we expect as a respsonse */
    unsigned char reply[MD_SIG_SIZE];   /* What the peer sent */
    unsigned char *vector;
    unsigned int vector_len;
    int state;                  /* What state is challenge in? */
};

extern struct lns *get_lns (struct tunnel *);
extern unsigned int get_addr (struct iprange *);
extern void reserve_addr (unsigned int);
extern void unreserve_addr (unsigned int);
extern void init_addr ();
extern int handle_challenge (struct tunnel *, struct challenge *);
extern void mk_challenge (unsigned char *, int);
#endif
