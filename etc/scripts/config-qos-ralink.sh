#!/bin/sh

#############################################################################################################
# Qos helper script for Wive-NG-RTNL									    #
#############################################################################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t Ralink QoS"

    $LOG "Load netsched  modules."
    # netsched modules
    sch_mod="cls_u32 cls_tcindex cls_fw act_gact act_police act_ipt em_cmp em_u32 em_nbyte sch_htb sch_dsmark"
    for module in $sch_mod
    do
	modprobe -q $module
    done
    # iptables modules
    ipt_mod="xt_connmark xt_conntrack xt_dscp xt_mark xt_DSCP xt_MARK xt_DSCP ipt_TOS sch_sfq sch_red sch_gred"
    for module in $ipt_mod
    do
	modprobe -q $module
    done
    qos_run
