/* 
 * Distributed under the terms of the GNU General Public License v2
 * $Header: //WIFI_SOC/release/SDK_4_1_0_0/source/lib/libc/sysdeps/linux/common/mincore.c#1 $
 *
 * This file provides the mincore() system call to uClibc.
 * 20041215 - <solar@gentoo.org>
 *
 */

#include "syscalls.h"
#include <unistd.h>

#ifdef __NR_mincore
_syscall3(int, mincore, void *, start, size_t, length, unsigned char *, vec);
#endif
