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
#if 1
/* macro to swap the bytes in a 16-bit variable */
#define swapbytes16(x) \
{ \
    unsigned short int data = *(unsigned short int*)&(x); \
    data = ((data & 0xff00) >> 8) |    \
           ((data & 0x00ff) << 8);     \
    *(unsigned short int*)&(x) = data;       \
}
/* macro to swap the bytes in a 32-bit variable */
#define swapbytes32(x) \
{ \
    unsigned int data = *(unsigned int*)&(x); \
    data = ((data & 0xff000000) >> 24) |    \
           ((data & 0x00ff0000) >>  8) |    \
           ((data & 0x0000ff00) <<  8) |    \
           ((data & 0x000000ff) << 24);     \
    *(unsigned int*)&(x) = data;            \
}
#else
#define endian_translate_s(A) ((((A) & 0xff00) >> 8) | \
                              ((A) & 0x00ff) << 8))
#define endian_translate_l(A) ((((A) & 0xff000000) >> 24) | \
                              (((A) & 0x00ff0000) >> 8) | \
                              (((A) & 0x0000ff00) << 8) | \
                              (((A) & 0x000000ff) << 24))
#endif

