#!/bin/sh

LOG="echo LANAUTH: "

#include global
. /etc/scripts/global.sh

start() {
    if [ -x /bin/lanauth ] && [ "$vpnEnabled" = "on" ]; then
	    reload
    fi
}

stop() {
 $LOG "Stopping lanauth "
 killall -q lanauth
 killall -q -9 lanauth
}

reload() {
    get_param
    if  [ "$pwd"  != "" ] && [ "$lvl" != "" ]; then
	if [ "$pid" != "0" ]; then
	    if [ "$lvl" = "1" ]; then
		killall -q -USR1 lanauth
	    elif [ "$lvl" = "2" ]; then
		killall -q -USR2 lanauth
	    else
		stop
	    fi
	else
    	    $LOG "Starting lanauth mode $lvl"
    	    lanauth -v 2 -l $lvl -p $pwd -A 0 &
	fi
    fi
}

get_param() {
    pwd=`nvram_get 2860 vpnPassword`
    lvl=`nvram_get 2860 LANAUTH_LVL`
    pid=`pidof lanauth`
}


case "$1" in
       start)
           start
           ;;

       stop)
           stop
           ;;

       restart)
           stop
           start
           ;;

	reload)
	   reload
	   ;;

       *)
           echo $"Usage: $0 {start|stop|restart|reload}"
           exit 1
esac


