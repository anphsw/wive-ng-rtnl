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
 *	ej.h -- Ejscript(TM) header
 *
 * Copyright (c) GoAhead Software Inc., 1992-2000. All Rights Reserved.
 *
 *	See the file "license.txt" for information on usage and redistribution
 *
 * $Id: ej.h,v 1.3 2007-02-01 07:41:01 winfred Exp $
 */

#ifndef _h_EJ
#define _h_EJ 1

/******************************** Description *********************************/

/* 
 *	GoAhead Ejscript(TM) header. This defines the Ejscript API and internal
 *	structures.
 */

/********************************* Includes ***********************************/

#ifndef UEMF
	#include	"basic/basic.h"
	#include	"emf/emf.h"
#else
	#include	"uemf.h"
#endif

/********************************** Defines ***********************************/

/******************************** Prototypes **********************************/

extern int 		ejArgs(int argc, char_t **argv, char_t *fmt, ...);
extern void		ejSetResult(int eid, char_t *s);
extern int		ejOpenEngine(sym_fd_t variables, sym_fd_t functions);
extern void		ejCloseEngine(int eid);
extern int 		ejSetGlobalFunction(int eid, char_t *name, 
					int (*fn)(int eid, void *handle, int argc, char_t **argv));
extern void		ejSetVar(int eid, char_t *var, char_t *value);
extern int		ejGetVar(int eid, char_t *var, char_t **value);
extern char_t	*ejEval(int eid, char_t *script, char_t **emsg);

#endif /* _h_EJ */

/*****************************************************************************/

