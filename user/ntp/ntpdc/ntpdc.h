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
 * ntpdc.h - definitions of interest to ntpdc
 */
#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_request.h"
#include "ntp_string.h"
#include "ntp_malloc.h"

/*
 * Maximum number of arguments
 */
#define	MAXARGS	4
#define	MOREARGS 10

/*
 * Flags for forming descriptors.
 */
#define	OPT		0x80	/* this argument is optional, or'd with type */

#define	NO		0x0
#define	NTP_STR		0x1	/* string argument */
#define	NTP_UINT	0x2	/* unsigned integer */
#define	NTP_INT		0x3	/* signed integer */
#define	NTP_ADD		0x4	/* IP network address */
#define IP_VERSION	0x5	/* IP version */

/*
 * Arguments are returned in a struct - no
 * union space saving is attempted. 
 */
typedef struct {
	u_char type;
	char *string;
	long ival;
	u_long uval;
	struct sockaddr_storage netnum;
} arg_v;

/*
 * Structure for passing parsed command line
 */
struct parse {
	char *keyword;
	arg_v argval[MAXARGS + MOREARGS];
	int nargs;
};

/*
 * ntpdc includes a command parser which could charitably be called
 * crude.  The following structure is used to define the command
 * syntax.
 */
struct xcmd {
  const char *keyword;		/* command key word */
	void (*handler)	P((struct parse *, FILE *));	/* command handler */
	u_char arg[MAXARGS];	/* descriptors for arguments */
  const char *desc[MAXARGS];	/* descriptions for arguments */
  const char *comment;
};

extern	int impl_ver;
extern	int showhostnames;
extern	int s_port;

extern	int	doquery	P((int, int, int, int, int, char *, int *, int *, char **, int, int));
extern	char *	nntohost	P((struct sockaddr_storage *));
