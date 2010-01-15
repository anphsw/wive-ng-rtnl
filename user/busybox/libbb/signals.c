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
/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 * Copyright (C) 2006 Rob Landley
 * Copyright (C) 2006 Denys Vlasenko
 *
 * Licensed under GPL version 2, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

/* Saves 2 bytes on x86! Oh my... */
int FAST_FUNC sigaction_set(int signum, const struct sigaction *act)
{
	return sigaction(signum, act, NULL);
}

int FAST_FUNC sigprocmask_allsigs(int how)
{
	sigset_t set;
	sigfillset(&set);
	return sigprocmask(how, &set, NULL);
}

void FAST_FUNC bb_signals(int sigs, void (*f)(int))
{
	int sig_no = 0;
	int bit = 1;

	while (sigs) {
		if (sigs & bit) {
			sigs &= ~bit;
			signal(sig_no, f);
		}
		sig_no++;
		bit <<= 1;
	}
}

void FAST_FUNC bb_signals_recursive(int sigs, void (*f)(int))
{
	int sig_no = 0;
	int bit = 1;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = f;
	/*sa.sa_flags = 0;*/
	/*sigemptyset(&sa.sa_mask); - hope memset did it*/

	while (sigs) {
		if (sigs & bit) {
			sigs &= ~bit;
			sigaction_set(sig_no, &sa);
		}
		sig_no++;
		bit <<= 1;
	}
}

void FAST_FUNC sig_block(int sig)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, sig);
	sigprocmask(SIG_BLOCK, &ss, NULL);
}

void FAST_FUNC sig_unblock(int sig)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, sig);
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void FAST_FUNC wait_for_any_sig(void)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigsuspend(&ss);
}

/* Assuming the sig is fatal */
void FAST_FUNC kill_myself_with_sig(int sig)
{
	signal(sig, SIG_DFL);
	sig_unblock(sig);
	raise(sig);
	_exit(EXIT_FAILURE); /* Should not reach it */
}

void FAST_FUNC signal_SA_RESTART_empty_mask(int sig, void (*handler)(int))
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	/*sigemptyset(&sa.sa_mask);*/
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
	sigaction_set(sig, &sa);
}

void FAST_FUNC signal_no_SA_RESTART_empty_mask(int sig, void (*handler)(int))
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	/*sigemptyset(&sa.sa_mask);*/
	/*sa.sa_flags = 0;*/
	sa.sa_handler = handler;
	sigaction_set(sig, &sa);
}
