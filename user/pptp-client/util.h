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
/* util.h ....... error message utilities.
 *                C. Scott Ananian <cananian@alumni.princeton.edu>
 *
 * $Id: util.h,v 1.1.1.1 2008/07/21 09:19:28 james26_jang Exp $
 */

#ifndef INC_UTIL_H
#define INC_UTIL_H

/* log_string is an identifier for this pptp process, passed from
   command line using --log-string=X, and included with every log message.
   Useful for people with multiple pptp sessions open at a time */
extern char * log_string;

/* log_level sets the logging verbosity. Values range from 0 (errors only)
   to 1 (errors and warnings) to 2 (high verbosity, for debugging) */
extern int    log_level;

void _log(const char *func, const char *file, int line, const char *format, ...)
     __attribute__ ((format (printf, 4, 5)));
void _warn(const char *func, const char *file, int line, const char *format, ...)
     __attribute__ ((format (printf, 4, 5)));
void _fatal(const char *func, const char *file, int line, const char *format, ...)
     __attribute__ ((format (printf, 4, 5))) __attribute__ ((noreturn));

#define log(format, args...) \
	_log(__FUNCTION__,__FILE__,__LINE__, format , ## args)
#define warn(format, args...) \
	_warn(__FUNCTION__,__FILE__,__LINE__, format , ## args)
#define fatal(format, args...) \
	_fatal(__FUNCTION__,__FILE__,__LINE__, format , ## args)

int file2fd(const char *path, const char *mode, int fd);

#endif /* INC_UTIL_H */
