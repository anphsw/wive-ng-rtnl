#############################################################################################################
# Qos helper script for Wive-NG-RTNL									    #
#############################################################################################################

# include global
. /etc/scripts/global.sh

LOG="logger -t User QoS"

    $LOG "Load netsched  modules."
    mod="imq ipt_IMQ"
    for module in $mod
    do
    	    modprobe -q $module
    done
    ip link set imq0 up > /dev/null 2>&1
    ip link set imq1 up > /dev/null 2>&1
