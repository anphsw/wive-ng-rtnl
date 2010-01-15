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
struct btfp_time                /* Structure for reading 5 time words   */
                                /* in one ioctl(2) operation.           */
{
  unsigned short btfp_time[5];  /* Time words 0,1,2,3, and 4. (16bit)*/
};

/***** Simple ioctl commands *****/

#define RUNLOCK   _IO('X',19)                   /* Release Capture Lockout */
#define RCR0      _IOR('X',22,unsigned int)     /* Read control register */
#define WCR0      _IOW('X',23,unsigned int)     /* Write control register */

/***** Compound ioctl commands *****/

/* Read all 5 time words in one call.   */
#define READTIME        _IOR('X',32,struct btfp_time)
#define VMEFD "/dev/btfp0"

 struct vmedate {               /* structure returned by get_vmetime.c */
         unsigned short year;
         unsigned short doy;
         unsigned short hr;
         unsigned short mn;
         unsigned short sec;
         unsigned long frac;
         unsigned short status;
         };

#define PRIO    120               /* set the realtime priority */
#define NREGS 7                    /* number of registers we will use */
