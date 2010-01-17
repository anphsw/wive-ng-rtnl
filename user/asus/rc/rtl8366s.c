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
#include <stdio.h>             
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
//#include <rtl8366s/smi.h>
//#include "rt2880_gpio.h"

#define SMI_DEV	"/dev/rtl8366s"

int smi_ctrl(int val)
{
        int fd;
        return 0;
}

void config_8366s(char *cmd)
{
}

int smi_test_1()
{
	int fd, val;
	return 0;
}

/*
int smi_test_2()
{
        int fd, val;
	return 0;
}
*/

/*
void usage(char *cmd)
{
        printf("Usage: %s 1 - set (SCK, SD) as (0, 1)\n", cmd);
        printf("       %s 2 - set (SCK, SD) as (1, 0)\n", cmd);
        printf("       %s 3 - set (SCK, SD) as (1, 1)\n", cmd);
        printf("       %s 0 - set (SCK, SD) as (0, 0)\n", cmd);
        printf("       %s 4 - init vlan\n", cmd);
        printf("       %s 5 - set cpu port 0 0\n", cmd);
        printf("       %s 6 - set cpu port 0 1\n", cmd);
        printf("       %s 7 - set cpu port 1 0\n", cmd);
        printf("       %s 8 - set cpu port 1 1\n", cmd);
        printf("       %s 10 - set vlan entry, no cpu port, but mbr\n", cmd);
        printf("       %s 11 - set vlan entry, no cpu port, no mbr\n", cmd);
        printf("       %s 15 - set vlan PVID, no cpu port\n", cmd);
        printf("       %s 20 - set vlan entry, with cpu port\n", cmd);
        printf("       %s 21 - set vlan entry, with cpu port and no cpu port in untag sets\n", cmd);
        printf("       %s 25 - set vlan PVID, with cpu port\n", cmd);
        printf("       %s 26 - set vlan PVID, not set cpu port\n", cmd);
        printf("       %s 90 - accept all frmaes\n", cmd);
        printf("       %s 66 - setup default vlan\n", cmd);
        printf("       %s 61 - setup vlan type1\n", cmd);
        printf("       %s 62 - setup vlan type2\n", cmd);
        printf("       %s 63 - setup vlan type3\n", cmd);
        printf("       %s 64 - setup vlan type4\n", cmd);
        printf("       %s 65 - setup vlan type34\n", cmd);
        printf("       %s 70 - disable multicast snooping\n", cmd);
        printf("       %s 81 - setRtctTesting on port x\n", cmd);
        printf("       %s 82 - getRtctResult on port x\n", cmd);
        printf("       %s 83 - setGreenEthernet x(green, powsav)\n", cmd);
        printf("       %s 84 - setAsicGreenFeature x(txGreen, rxGreen)\n", cmd);
        printf("       %s 85 - getAsicGreenFeature\n", cmd);
        printf("       %s 86 - enable GreenEthernet on port x\n", cmd);
        printf("       %s 87 - disable GreenEthernet on port x\n", cmd);
        printf("       %s 88 - getAsicPowerSaving x\n", cmd);
        printf("       %s 50 - getPortLinkStatus x\n", cmd);
	exit(0);
}
*/

