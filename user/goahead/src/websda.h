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
 *	websda.h -- GoAhead Digest Access Authentication public header
 *
 * Copyright (c) GoAhead Software Inc., 1992-2000. All Rights Reserved.
 *
 *	See the file "license.txt" for information on usage and redistribution
 *
 * $Id: websda.h,v 1.3 2007-02-01 07:41:01 winfred Exp $
 */

#ifndef _h_WEBSDA
#define _h_WEBSDA 1

/******************************** Description *********************************/

/* 
 *	GoAhead Digest Access Authentication header. This defines the Digest 
 *	access authentication public APIs.  Include this header for files that 
 *	use DAA functions
 */

/********************************* Includes ***********************************/

#ifndef UEMF
	#include	"basic/basic.h"
	#include	"emf/emf.h"
#else
	#include	"uemf.h"
#endif

#include		"webs.h"

/****************************** Definitions ***********************************/

extern char_t 	*websCalcNonce(webs_t wp);
extern char_t 	*websCalcOpaque(webs_t wp);
extern char_t 	*websCalcDigest(webs_t wp);

#endif /* _h_WEBSDA */

/******************************************************************************/

