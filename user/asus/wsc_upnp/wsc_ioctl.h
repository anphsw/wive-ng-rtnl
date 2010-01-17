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
	header file for wsc_ioctl

*/
#ifndef __WSC_IOCTL_H__
#define __WSC_IOCTL_H__


#if WIRELESS_EXT <= 11
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE                              0x8BE0
#endif
#define SIOCIWFIRSTPRIV								SIOCDEVPRIVATE
#endif

#define RT_PRIV_IOCTL								(SIOCIWFIRSTPRIV + 0x01)

#define OID_GET_SET_TOGGLE							0x8000

#define RT_OID_WSC_UUID								0x0753
#define RT_OID_WSC_SET_SELECTED_REGISTRAR			0x0754
#define RT_OID_WSC_EAPMSG							0x0755

#endif
