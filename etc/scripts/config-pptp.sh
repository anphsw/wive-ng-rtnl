#!/bin/sh

killall -q -9 pppd > /dev/null 2>&1
killall -q -9 xl2tpd > /dev/null 2>&1
LOG="logger -t vpnhelper"

echo "==================START-PPTP-CLIENT======================="
#clear all configs
ppp=/etc/ppp
echo > $ppp/chap-secrets
echo > $ppp/pap-secrets

    SERVER=`nvram_get 2860 wan_pptp_server`
    USER=`nvram_get 2860 wan_pptp_user`
    PASSWORD=`nvram_get 2860 wan_pptp_pass`

    $LOG "Check for PPTP server reachable"
    reachable=0;
    while [ $reachable -eq 0 ]; do
        ping -q -c 1 $SERVER
        if [ "$?" -eq 0 ]; then
            reachable=1
        else
            $LOG "Server unreachable wait 30 sec."
            sleep 30
            reachable=0;
        fi
    done

    $LOG "Get vpn server ip address."
    ADDRESS=`nslookup "$SEVER" | grep Address | tail -n1 | cut -c 12- | awk {' print $1 '}`
    if [ "$ADDRESS" != "" ]; then
        SERVER=$ADDRESS
     else
	SERVER=$SERVER
    fi

    $LOG "Get route to vpn server."
    ROUTE=`ip r get "$SERVER" | grep dev | cut -f -3 -d " "`
    if [ "$ROUTE" != "" ] || [ "$ROUTE" != "0.0.0.0" ]; then
	echo "Add route to vpn server."
	ip r add $ROUTE
    fi

    $LOG "Remove default route"
    ip route del default 2> /dev/null

    $LOG "PPTP connect to $SERVER ....."
    $LOG "Start pppd"
    pppd file /etc/ppp/options.pptp -detach mtu 1400 mru 1400 plugin /lib/pptp.so allow-mppe-128 \
    pptp_server $SERVER call pptp defaultroute persist usepeerdns user $USER password $PASSWORD &
