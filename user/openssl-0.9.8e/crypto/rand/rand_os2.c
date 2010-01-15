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
/* crypto/rand/rand_os2.c */
/* ====================================================================
 * Copyright (c) 1998-2000 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#include "cryptlib.h"
#include <openssl/rand.h>
#include "rand_lcl.h"

#ifdef OPENSSL_SYS_OS2

#define INCL_DOSPROCESS
#define INCL_DOSPROFILE
#define INCL_DOSMISC
#define INCL_DOSMODULEMGR
#include <os2.h>

#define   CMD_KI_RDCNT    (0x63)

typedef struct _CPUUTIL {
    ULONG ulTimeLow;            /* Low 32 bits of time stamp      */
    ULONG ulTimeHigh;           /* High 32 bits of time stamp     */
    ULONG ulIdleLow;            /* Low 32 bits of idle time       */
    ULONG ulIdleHigh;           /* High 32 bits of idle time      */
    ULONG ulBusyLow;            /* Low 32 bits of busy time       */
    ULONG ulBusyHigh;           /* High 32 bits of busy time      */
    ULONG ulIntrLow;            /* Low 32 bits of interrupt time  */
    ULONG ulIntrHigh;           /* High 32 bits of interrupt time */
} CPUUTIL;

APIRET APIENTRY(*DosPerfSysCall) (ULONG ulCommand, ULONG ulParm1, ULONG ulParm2, ULONG ulParm3) = NULL;
APIRET APIENTRY(*DosQuerySysState) (ULONG func, ULONG arg1, ULONG pid, ULONG _res_, PVOID buf, ULONG bufsz) = NULL;
HMODULE hDoscalls = 0;

int RAND_poll(void)
{
    char failed_module[20];
    QWORD qwTime;
    ULONG SysVars[QSV_FOREGROUND_PROCESS];

    if (hDoscalls == 0) {
        ULONG rc = DosLoadModule(failed_module, sizeof(failed_module), "DOSCALLS", &hDoscalls);

        if (rc == 0) {
            rc = DosQueryProcAddr(hDoscalls, 976, NULL, (PFN *)&DosPerfSysCall);

            if (rc)
                DosPerfSysCall = NULL;

            rc = DosQueryProcAddr(hDoscalls, 368, NULL, (PFN *)&DosQuerySysState);

            if (rc)
                DosQuerySysState = NULL;
        }
    }

    /* Sample the hi-res timer, runs at around 1.1 MHz */
    DosTmrQueryTime(&qwTime);
    RAND_add(&qwTime, sizeof(qwTime), 2);

    /* Sample a bunch of system variables, includes various process & memory statistics */
    DosQuerySysInfo(1, QSV_FOREGROUND_PROCESS, SysVars, sizeof(SysVars));
    RAND_add(SysVars, sizeof(SysVars), 4);

    /* If available, sample CPU registers that count at CPU MHz
     * Only fairly new CPUs (PPro & K6 onwards) & OS/2 versions support this
     */
    if (DosPerfSysCall) {
        CPUUTIL util;

        if (DosPerfSysCall(CMD_KI_RDCNT, (ULONG)&util, 0, 0) == 0) {
            RAND_add(&util, sizeof(util), 10);
        }
        else {
            DosPerfSysCall = NULL;
        }
    }

    /* DosQuerySysState() gives us a huge quantity of process, thread, memory & handle stats */
    if (DosQuerySysState) {
        char *buffer = OPENSSL_malloc(256 * 1024);

        if (DosQuerySysState(0x1F, 0, 0, 0, buffer, 256 * 1024) == 0) {
            /* First 4 bytes in buffer is a pointer to the thread count
             * there should be at least 1 byte of entropy per thread
             */
            RAND_add(buffer, 256 * 1024, **(ULONG **)buffer);
        }

        OPENSSL_free(buffer);
        return 1;
    }

    return 0;
}

#endif /* OPENSSL_SYS_OS2 */
