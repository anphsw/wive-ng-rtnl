#!/bin/sh

killall -q -9 pppd > /dev/null 2>&1
killall -q -9 xl2tpd > /dev/null 2>&1

echo "==================START-PPTP-CLIENT======================="
#clear all configs
ppp=/etc/ppp
echo > $ppp/chap-secrets
echo > $ppp/pap-secrets

    SERVER=`nvram_get 2860 wan_pptp_server`
    USER=`nvram_get 2860 wan_pptp_user`
    PASSWORD=`nvram_get 2860 wan_pptp_pass`

    echo "Check for server reachable"
    reachable=0;
    while [ $reachable -eq 0 ]; do
        ping -q -c 1 $SERVER
        if [ "$?" -eq 0 ]; then
            reachable=1
        else
            echo "Server unreachable wait 30 sec."
            sleep 30
            reachable=0;
        fi
    done

    echo "Add static route to vpn server."
    ADDRESS=`nslookup "$SEVER" | grep Address | tail -n1 | cut -c 12- | awk {' print $1 '}`
    if [ "$ADDRESS" != "" ]; then
        SERVER=$ADDRESS
     else
	SERVER=$SERVER
    fi
    ROUTE=`ip r get "$SERVER" | grep dev | cut -f -3 -d " "`
    ip r add $ROUTE

    echo "Remove default route"
    ip route del default 2> /dev/null

    echo "PPTP connect to $SERVER ....."

    pppd file /etc/ppp/options.pptp -detach mtu 1400 mru 1400 plugin /lib/pptp.so allow-mppe-128 \
    pptp_server $SERVER call pptp defaultroute persist usepeerdns user $USER password $PASSWORD &
