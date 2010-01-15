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
 * $Id: strdup.c,v 1.1.1.1 2007-08-22 05:38:26 steven Exp $
 *
 * Copyright (C) 1996 Lars Fenneberg and Christian Graefe
 *
 * This file is provided under the terms and conditions of the GNU general
 * public license, version 2 or any later version, incorporated herein by
 * reference.
 *
 */

#include "config.h"
#include "includes.h"

/*
 * Function: strdup
 *
 * Purpose:  strdup replacement for systems which lack it
 *
 */

char *strdup(char *str)
{
	char *p;

	if (str == NULL)
		return NULL;

	if ((p = (char *)malloc(strlen(str)+1)) == NULL)
		return p;

	return strcpy(p, str);
}
