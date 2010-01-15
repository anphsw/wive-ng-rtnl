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
 * arlib.h (C)opyright 1992 Darren Reed.
 */

#define	ARES_INITLIST	1
#define	ARES_CALLINIT	2
#define ARES_INITSOCK	4
#define ARES_INITDEBG	8
#define ARES_INITCACH	16

#ifdef	__STDC__
extern	struct	hostent	*ar_answer(char *, int);
extern	void    ar_close();
extern	int     ar_delete(char *, int);
extern	int     ar_gethostbyname(char *, char *, int);
extern	int     ar_gethostbyaddr(char *, char *, int);
extern	int     ar_init(int);
extern	int     ar_open();
extern	long    ar_timeout(time_t, char *, int);
#else
extern	struct	hostent	*ar_answer();
extern	void    ar_close();
extern	int     ar_delete();
extern	int     ar_gethostbyname();
extern	int     ar_gethostbyaddr();
extern	int     ar_init();
extern	int     ar_open();
extern	long    ar_timeout();
#endif
