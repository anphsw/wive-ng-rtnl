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
/* ppsapi_timepps.h */

/*
 * This logic first tries to get the timepps.h file from a standard
 * location, and then from our include/ subdirectory.
 */

#ifdef HAVE_TIMEPPS_H
# include <timepps.h>
#else
# ifdef HAVE_SYS_TIMEPPS_H
#  include <sys/timepps.h>
# else
#  ifdef HAVE_CIOGETEV
#   include "timepps-SunOS.h"
#  else
#   ifdef HAVE_TIOCGPPSEV
#    include "timepps-Solaris.h"
#   else
#    ifdef TIOCDCDTIMESTAMP
#     include "timepps-SCO.h"
#    endif
#   endif
#  endif
# endif
#endif
