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
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include "globals.h"


static int get_sockfd(void)
{
   static int sockfd = -1;

   if (sockfd == -1)
   {
      if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
      {
         perror("user: socket creating failed");
         return (-1);
      }
   }
   return sockfd;
}

int GetIpAddressStr(char *address, char *ifname)
{
   struct ifreq ifr;
   struct sockaddr_in *saddr;
   int fd;
   int succeeded = 0;

   fd = get_sockfd();
   if (fd >= 0 )
   {
      strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
      ifr.ifr_addr.sa_family = AF_INET;
      if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
      {
         saddr = (struct sockaddr_in *)&ifr.ifr_addr;
         strcpy(address,inet_ntoa(saddr->sin_addr));
         succeeded = 1;
      }
      else
      {
         syslog(LOG_ERR, "Failure obtaining ip address of interface %s", ifname);
         succeeded = 0;
      }
   }
   return succeeded;
}

void trace(int debuglevel, const char *format, ...)
{
  va_list ap;
  va_start(ap,format);
  if (g_vars.debug>=debuglevel) {
    vsyslog(LOG_DEBUG,format,ap);
  }
  va_end(ap);
}
