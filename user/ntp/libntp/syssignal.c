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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#include "ntp_syslog.h"
#include "ntp_stdlib.h"

#ifdef HAVE_SIGACTION

void
signal_no_reset(
#if defined(__STDC__) || defined(HAVE_STDARG_H)
	int sig,
	void (*func) (int)
#else
	sig, func
#endif
	)
#if defined(__STDC__) || defined(HAVE_STDARG_H)
#else
	 int sig;
	 void (*func) P((int));
#endif
{
	int n;
	struct sigaction vec;

	vec.sa_handler = func;
	sigemptyset(&vec.sa_mask);
#if 0
#ifdef SA_RESTART
	vec.sa_flags = SA_RESTART;
#else
	vec.sa_flags = 0;
#endif
#else
	vec.sa_flags = 0;
#endif

#ifdef SA_RESTART
/* Added for PPS clocks on Solaris 7 which get EINTR errors */
# ifdef SIGPOLL
	if (sig == SIGPOLL) vec.sa_flags = SA_RESTART;
# endif
# ifdef SIGIO
	if (sig == SIGIO)   vec.sa_flags = SA_RESTART;
# endif
#endif

	while (1)
	{
		struct sigaction ovec;

		n = sigaction(sig, &vec, &ovec);
		if (n == -1 && errno == EINTR) continue;
		if (ovec.sa_flags
#ifdef	SA_RESTART
		    && ovec.sa_flags != SA_RESTART
#endif
		    )
			msyslog(LOG_DEBUG, "signal_no_reset: signal %d had flags %x",
				sig, ovec.sa_flags);
		break;
	}
	if (n == -1) {
		perror("sigaction");
		exit(1);
	}
}

#elif  HAVE_SIGVEC

void
signal_no_reset(
	int sig,
	RETSIGTYPE (*func) (int)
	)
{
	struct sigvec sv;
	int n;

	bzero((char *) &sv, sizeof(sv));
	sv.sv_handler = func;
	n = sigvec(sig, &sv, (struct sigvec *)NULL);
	if (n == -1) {
		perror("sigvec");
		exit(1);
	}
}

#elif  HAVE_SIGSET

void
signal_no_reset(
	int sig,
	RETSIGTYPE (*func) (int)
	)
{
	int n;

	n = sigset(sig, func);
	if (n == -1) {
		perror("sigset");
		exit(1);
	}
}

#else

/* Beware!	This implementation resets the signal to SIG_DFL */
void
signal_no_reset(
	int sig,
	RETSIGTYPE (*func) (int)
	)
{
#ifdef SIG_ERR
	if (SIG_ERR == signal(sig, func)) {
#else
	int n;
	n = signal(sig, func);
	if (n == -1) {
#endif
		perror("signal");
		exit(1);
	}
}

#endif
