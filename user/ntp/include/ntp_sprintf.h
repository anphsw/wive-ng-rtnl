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
 * Handle ancient char* *s*printf*() systems
 */

#ifdef SPRINTF_CHAR
# define SPRINTF(x)	strlen(sprintf/**/x)
# define SNPRINTF(x)	strlen(snprintf/**/x)
# define VSNPRINTF(x)	strlen(vsnprintf/**/x)
#else
# define SPRINTF(x)	((size_t)sprintf x)
# define SNPRINTF(x)	((size_t)snprintf x)
# define VSNPRINTF(x)	((size_t)vsnprintf x)
#endif
