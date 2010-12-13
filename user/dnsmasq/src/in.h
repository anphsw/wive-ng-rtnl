/* Copyright (C) 1991-1999, 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef	_NETINET_IN_H
#define	_NETINET_IN_H	1

#include <features.h>
#include <stdint.h>
#include <bits/types.h>

__BEGIN_DECLS

/* Functions to convert between host and network byte order.

   Please note that these functions normally take `unsigned long int' or
   `unsigned short int' values as arguments and also return them.  But
   this was a short-sighted decision since on different systems the types
   may have different representations but the values are always the same.  */

extern uint32_t ntohl (uint32_t __netlong) __THROW __attribute__ ((__const__));
extern uint16_t ntohs (uint16_t __netshort) __THROW __attribute__ ((__const__));
extern uint32_t htonl (uint32_t __hostlong) __THROW __attribute__ ((__const__));
extern uint16_t htons (uint16_t __hostshort) __THROW __attribute__ ((__const__));

/* Internet address.  */                                                                                                                    
typedef uint32_t in_addr_t;                                                                                                                 

__END_DECLS

#endif	/* netinet/in.h */
