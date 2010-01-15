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
 * ntp_filegen.h,v 3.9 1996/12/01 16:02:45 kardel Exp
 *
 * definitions for NTP file generations support
 *
 *
 * Copyright (C) 1992, 1996 by Rainer Pruy
 * Friedrich-Alexander Universität Erlangen-Nürnberg, Germany
 *
 * This code may be modified and used freely
 * provided the credits remain intact.
 */

#include "ntp_types.h"

/*
 * supported file generation types
 */

#define FILEGEN_NONE	255	/* no generations - use plain file name */
#define FILEGEN_PID	1	/* one filegen per process incarnation */
#define FILEGEN_DAY	2	/* one filegen per day */
#define FILEGEN_WEEK	3	/* one filegen per week */
#define FILEGEN_MONTH	4	/* one filegen per month */
#define FILEGEN_YEAR	5	/* one filegen per year */
#define FILEGEN_AGE     6	/* change filegen each FG_AGE_SECS */

/*
 * supported file generation flags
 */

#define FGEN_FLAG_LINK		0x01 /* make a link to base name */

#define FGEN_FLAG_ENABLED	0x80 /* set this to really create files   */
				     /* without this, open is suppressed */

typedef struct FILEGEN
        {
	  FILE   *fp;		/* file referring to current generation */
	  char   *prefix;	/* filename prefix and basename to be used*/
	  char   *basename;	/* for constructing filename of generation file */
				/* WARNING: must be malloced !!! will be fed to free()*/
	  u_long  id;		/* id of current generation */
	  u_char  type;		/* type of file generation */
	  u_char  flag;		/* flags modifying processing of file generation */
	}	FILEGEN;

extern	void	filegen_setup	P((FILEGEN *, u_long));
extern	void	filegen_config	P((FILEGEN *, char *, u_int, u_int));
extern	FILEGEN *filegen_get	P((char *));
extern	void	filegen_register P((char *, const char *, FILEGEN *));
