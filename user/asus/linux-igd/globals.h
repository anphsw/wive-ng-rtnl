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
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <net/if.h>

#define CHAIN_NAME_LEN 32
#define BITRATE_LEN 32
#define PATH_LEN 64
#define RESULT_LEN 512
#define NUM_LEN 32

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

struct GLOBALS {
  char extInterfaceName[IFNAMSIZ]; // The name of the external interface, picked up from the
                                   // command line
  char intInterfaceName[IFNAMSIZ]; // The name of the internal interface, picked from command line

  // All vars below are read from /etc/upnpd.conf in main.c
  int debug;  // 1 - print debug messages to syslog
               // 0 - no debug messages
  char iptables[PATH_LEN];  // The full name and path of the iptables executable, used in pmlist.c
  char upstreamBitrate[BITRATE_LEN];  // The upstream bitrate reported by the daemon
  char downstreamBitrate[BITRATE_LEN]; // The downstream bitrate reported by the daemon
  char forwardChainName[CHAIN_NAME_LEN];  // The name of the iptables chain to put FORWARD rules in
  char preroutingChainName[CHAIN_NAME_LEN]; // The name of the chain to put PREROUTING rules in
  int forwardRules;     // 1 - forward rules are inserted
                          // 0 - no forward rules inserted
  long int duration;    // 0 - no duration
                          // >0 - duration in seconds
                          // <0 - expiration time 
  char descDocName[PATH_LEN];
  char xmlPath[PATH_LEN];
};

typedef struct GLOBALS* globals_p;
extern struct GLOBALS g_vars;

#define CONF_FILE "/etc_ro/linuxigd/upnpd.conf"
#define MAX_CONFIG_LINE 256
#define IPTABLES_DEFAULT_FORWARD_CHAIN "FORWARD"
#define IPTABLES_DEFAULT_PREROUTING_CHAIN "PREROUTING"
#define DEFAULT_DURATION 0
#define DEFAULT_UPSTREAM_BITRATE "0"
#define DEFAULT_DOWNSTREAM_BITRATE "0"
#define DESC_DOC_DEFAULT "gatedesc.xml"
#define XML_PATH_DEFAULT "/etc/linuxigd"

#endif // _GLOBALS_H_
