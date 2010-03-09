#!/bin/sh

killall -q pppd > /dev/null 2>&1
killall -q xl2tpd > /dev/null 2>&1
LOG="logger -t vpnhelper"

if [ -f /etc/ppp/ip-down-route-reload ]; then
    $LOG "Load old dgw from file"
    /etc/ppp/ip-down-route-reload
    rm -f /etc/ppp/ip-down-route-reload
fi

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
        ping -q -c 1 "$SERVER"
        if [ "$?" -eq 0 ]; then
            reachable=1
        else
            $LOG "Server unreachable wait 30 sec."
            sleep 30
            reachable=0;
        fi
    done

    $LOG "Get vpn server ip adress"
    NS=`nslookup "$SERVER" 2>&1`
    RESOLVEOK=`echo "$NS" | grep -c "can't resolve"`
    if [ "$RESOLVEOK" = "0" ]; then
        ADDRESS=`echo "$NS" | grep Address | tail -n1 | cut -c 12- | awk {' print $1 '}`
        $LOG "Server adress is $ADDRESS"
        SERVER="$ADDRESS"
    else
        $LOG "Not resolve adress for $SERVER"
        SERVER="$SERVER"
    fi

    $LOG "Get route to vpn server."
    ROUTE=`ip r get "$SERVER" | grep dev | cut -f -3 -d " "`
    if [ "$ROUTE" != "" ] || [ "$ROUTE" != "0.0.0.0" ]; then
	echo "Add route to vpn server."
	ip r add "$ROUTE"
    fi

    $LOG "PPTP connect to $SERVER ....."
    $LOG "Start pppd"
    FULLOPT="file /etc/ppp/options.pptp -detach mtu 1400 mru 1400 plugin /lib/pptp.so allow-mppe-128 \
    pptp_server $SERVER call pptp persist usepeerdns user $USER password $PASSWORD"
    pppd $FULLOPT &
