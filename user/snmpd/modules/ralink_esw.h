/*
 * 
 * Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 */

#include <linux/config.h>

#ifndef	RALINK_ESW_MIB
#define	RALINK_ESW_MIB

#if defined(CONFIG_RALINK_RT2880)
#define PROCREG_SNMP	"/proc/rt2880/snmp"
#elif defined(CONFIG_RALINK_RT3052)
#define PROCREG_SNMP	"/proc/rt3052/snmp"
#elif defined(CONFIG_RALINK_RT3352)
#define PROCREG_SNMP	"/proc/rt3352/snmp"
#elif defined (CONFIG_RALINK_RT5350)
#define PROCREG_SNMP	"/proc/rt5350/snmp"
#elif defined(CONFIG_RALINK_RT2883)
#define PROCREG_SNMP	"/proc/rt2883/snmp"
#elif defined(CONFIG_RALINK_RT3883)
#define PROCREG_SNMP	"/proc/rt3883/snmp"
#elif defined (CONFIG_RALINK_RT6855)
#define PROCREG_SNMP	"/proc/rt6855/snmp"
#elif defined (CONFIG_RALINK_RT63365)
#define PROCREG_SNMP	"/proc/rt63365/snmp"
#else
#define PROCREG_SNMP	"/proc/rt2880/snmp"
#endif

CVoidType ralink_esw_init(void);

#define ESW_MAXTYPE	17
#define ESW_OID		"\53\6\1\4\1\1107\1\1\1\1"

#define	RALINKESW_CDMAFCCFG	0
#define	RALINKESW_GDMA1FCCFG	1
#define	RALINKESW_PDMAFCCFG	2
#define	RALINKESW_GDMA1SCHCFG	3
#define	RALINKESW_GDMA2SCHCFG	4
#define	RALINKESW_PDMASCHCFG	5
#define	RALINKESW_GDMAGBCNT0	6
#define	RALINKESW_GDMAGPCNT0	7
#define	RALINKESW_GDMAOERCNT0	8
#define	RALINKESW_GDMAFERCNT0	9
#define	RALINKESW_GDMASERCNT0	10
#define	RALINKESW_GDMALERCNT0	11
#define	RALINKESW_GDMACERCNT0	12

#define	RALINKESW_PORT0CNT	13
#define	RALINKESW_PORT1CNT	14
#define	RALINKESW_PORT2CNT	15
#define	RALINKESW_PORT3CNT	16
#define	RALINKESW_PORT4CNT	17
#define	RALINKESW_PORT5CNT	18
#endif
