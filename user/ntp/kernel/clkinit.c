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
**	dynamically loadable clk driver
**
**	william robertson <rob@agate.berkeley.edu>  
*/

#include <sys/types.h>
#include <sys/conf.h>
#include <sys/errno.h>
#include <sys/stream.h>
#include <sys/syslog.h>

#include <sun/openprom.h>
#include <sun/vddrv.h>

extern int findmod();		/* os/str_io.c */

extern struct streamtab clkinfo;

struct vdldrv vd = {
     VDMAGIC_USER,
     "clk"
  };


int
xxxinit(function_code, vdp, vdi, vds)
unsigned int function_code;
struct vddrv *vdp;
addr_t vdi;
struct vdstat *vds;
{
     register int i = 0;
     register int j;

     switch (function_code) {
	case VDLOAD:

	  if (findmod("clk") >= 0) {
	       log(LOG_ERR, "clk stream module already loaded\n");
	       return (EADDRINUSE);
	  }
 
	  i = findmod("\0");

	  if (i == -1 || fmodsw[i].f_name[0] != '\0') 
	    return(-1);

	  for (j = 0; vd.Drv_name[j] != '\0'; j++)	/* XXX check bounds */
	    fmodsw[i].f_name[j] = vd.Drv_name[j];

	  fmodsw[i].f_name[j] = '\0';
	  fmodsw[i].f_str = &clkinfo;

	  vdp->vdd_vdtab = (struct vdlinkage *)  &vd;

	  return(0);

	case VDUNLOAD:
	  if ((i = findmod(vd.Drv_name)) == -1)
	    return(-1);

	  fmodsw[i].f_name[0] = '\0';
	  fmodsw[i].f_str = 0;

	  return(0);
     
	case VDSTAT:
	  return(0);

	default:
	  return(EIO);
     }
}
