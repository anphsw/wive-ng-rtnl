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
 * mexit - Used to exit the NTPD daemon
 * 
 */

#include <windows.h>
#include <stdio.h>

HANDLE hServDoneEvent = NULL;

void
service_exit(
	int status
	)
{
	extern int debug;

	if (debug) /* did not become a service, simply exit */
	    ExitThread((DWORD)status);
	else {
		/* service mode, need to have the service_main routine
		 * register with the service control manager that the 
		 * service has stopped running, before exiting
		 */
		if ((status > 0) && (hServDoneEvent != NULL))
		    SetEvent(hServDoneEvent);
		ExitThread((DWORD)status);
	}
}

