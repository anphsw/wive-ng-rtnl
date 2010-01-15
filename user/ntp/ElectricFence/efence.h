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
#include <sys/types.h>
#include <sys/param.h>

/*
 * ef_number is the largest unsigned integer we'll need. On systems that
 * support 64-bit pointers, this may be "unsigned long long".
 */
#if defined(USE_LONG_LONG)
typedef unsigned long long	ef_number;
#else
typedef unsigned long		ef_number;
#endif

/*
 * NBBY is the number of bits per byte. Some systems define it in
 * <sys/param.h> .
 */
#ifndef	NBBY
#define	NBBY	8
#endif

/*
 * This is used to declare functions with "C" linkage if we are compiling
 * with C++ .
 */
#ifdef	__cplusplus
#define	C_LINKAGE	"C"
#else
#define	C_LINKAGE
#endif

void			Page_AllowAccess(void * address, size_t size);
void *			Page_Create(size_t size);
void			Page_Delete(void * address, size_t size);
void			Page_DenyAccess(void * address, size_t size);
size_t			Page_Size(void);

void			EF_Abort(const char * message, ...);
void			EF_Exit(const char * message, ...);
void			EF_Print(const char * message, ...);
void			EF_Lock();
void			EF_UnLock();
